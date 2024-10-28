#pragma once
#include "common/common.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <list>
#include <functional>
#include <algorithm>
namespace event {
template <class ...params>
class async {
public:
    using handler = std::function<void(params...)>;
    using handler_id = std::list<handler>::iterator;
    struct signal {
        std::list<handler> handlers;
        void disconnect(handler_id id) {
            handlers.erase(id);
        };
    };
private:
    std::shared_ptr<signal> signal_;
public:
    async(): signal_(std::make_shared<signal>()) {}
    virtual ~async() = default;
    class connection {
        friend async;
        std::weak_ptr<async::signal> signal_;
        handler_id id_;
        bool persist_mode_;
    public:
	    connection(const connection& other) = delete;
	    connection(connection&& other) noexcept:
            signal_(other.signal_), id_(other.id_), persist_mode_(other.persist_mode_) {
            other.signal_.reset();
        }
	    connection& operator=(const connection& other) = delete;
	    connection& operator=(connection&& other) noexcept {
            signal_ = other.signal_;
            id_ = other.id_;
            persist_mode_ = other.persist_mode_;
            other.signal_.reset();
            return *this;
        }
        connection(handler_id id, const std::shared_ptr<signal>& signal, bool persist_mode = false):
           signal_(signal), id_(id), persist_mode_(persist_mode) {}
        connection(): signal_(nullptr) {}
        ~connection() {
            if (not persist_mode_) disconnect();
        }
        void disconnect() {
            if (auto ev = signal_.lock()) { ev->disconnect(id_); }
        }
    };
    connection connect(handler&& fn, bool persist_connection = false) {
        auto& handlers = signal_->handlers;
        auto id = handlers.insert(handlers.end(), std::forward<handler>(fn));
        return connection{id, signal_, persist_connection};
    }
    void disconnect(handler_id id) {
        signal_->disconnect(id);
    };
    void disconnect_all() {
        signal_->handlers.clear();
    }
    void fire(params ...args) {
        for (handler& handler : signal_->handlers) {
            handler(args...);
        }
    }
};
struct opaque_connection;
struct opaque_event;
void process_event_stack_entries(int amount = 10);
struct opaque_signal {
    virtual ~opaque_signal() = default;
    std::vector<std::unique_ptr<opaque_connection>> connections;
    void disconnect(opaque_connection* p);
};
struct opaque_event {
    virtual ~opaque_event() = default;
    opaque_event();
    std::shared_ptr<opaque_signal> signal;
};
struct opaque_connection {//has weird semantics, deal with it
    virtual ~opaque_connection();
    opaque_connection(const opaque_connection& other);
    opaque_connection& operator=(const opaque_connection& other);
    opaque_connection(opaque_connection&& other) noexcept;
    opaque_connection& operator=(opaque_connection&& other) noexcept;
    uintptr_t opaque_handler;//casting the function pointer to a uintptr_t as generic address, more platform independent approach compared to void*
    std::weak_ptr<opaque_signal> signal;
    bool auto_disconnect{true};
    opaque_connection(const std::shared_ptr<opaque_signal>& signal, uintptr_t opaque_handler);
    void disconnect();
    virtual void receive(void* data) = 0;
};
namespace intern {
struct event_stack_entry {
    int32_t pool_address;
    void* data;
};
inline common::flat_stack<event_stack_entry> event_stack;
inline common::lazy_pool<opaque_connection*> connection_pool;
}/*intern end*/
template <class param>
struct connection: public opaque_connection {
    using handler = void(*)(const param&);
    connection(handler fn, const std::shared_ptr<opaque_signal>& signal):
        opaque_connection(signal, reinterpret_cast<uintptr_t>(fn)) {
    }
    virtual void receive(void* data) override {
        reinterpret_cast<handler>(opaque_handler)(*static_cast<param*>(data));
    }
};
template <>
struct connection<void>: public opaque_connection {
    using handler = void(*)();
    connection(handler fn, const std::shared_ptr<opaque_signal>& signal):
        opaque_connection(signal, reinterpret_cast<uintptr_t>(fn)) {
    }
    virtual void receive(void* data) override {
        common::print("received");
        reinterpret_cast<handler>(opaque_handler)();
    }
};
template <class param>
struct event: public opaque_event {
    param data;
    void send() {
        for (auto& c : signal->connections) {
            const size_t old_capacity = intern::event_stack.capacity();
            intern::event_stack.push({.pool_address = intern::connection_pool.allocate([&c](opaque_connection*& p){
                if (p == nullptr) {
                    p = new connection<param>{*static_cast<connection<param>*>(c.get())};
                } else {
                    p->~opaque_connection();
                    new (p) connection<param>{*static_cast<connection<param>*>(c.get())};
                }
            }), .data = &data});
        }
    }
    connection<param>& connect(const connection<param>::handler& fn) {
        signal->connections.emplace_back(std::make_unique<connection<param>>(fn, signal));
        return static_cast<connection<param>&>(*signal->connections.back());
    }
};
template <>
struct event<void>: public opaque_event {
    void send() {
        for (auto& c : signal->connections) {
            const size_t old_capacity = intern::event_stack.capacity();
            intern::event_stack.push({.pool_address = intern::connection_pool.allocate([&c](opaque_connection*& p){
                if (p == nullptr) {
                    p = new connection<void>{*static_cast<connection<void>*>(c.get())};
                } else {
                    p->~opaque_connection();
                    new (p) connection<void>{*static_cast<connection<void>*>(c.get())};
                }
            }), .data = nullptr});
        }
    }
    connection<void>& connect(const connection<void>::handler& fn) {
        signal->connections.emplace_back(std::make_unique<connection<void>>(fn, signal));
        return static_cast<connection<void>&>(*signal->connections.back());
    }
};
}
