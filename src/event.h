#pragma once
#include "common.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <list>
#include <functional>
#include <algorithm>
namespace event {
template <class ...Params>
class Async {
public:
    using Handler = std::function<void(Params...)>;
    using Handler_id = std::list<Handler>::iterator;
    struct Signal {
        std::list<Handler> handlers;
        void disconnect(Handler_id id) {
            handlers.erase(id);
        };
    };
private:
    std::shared_ptr<Signal> signal_;
public:
    Async(): signal_(std::make_shared<Signal>()) {}
    virtual ~Async() = default;
    class Connection {
        friend Async;
        std::weak_ptr<Async::Signal> signal_;
        Handler_id id_;
        bool persist_mode_;
    public:
	    Connection(const Connection& other) = delete;
	    Connection(Connection&& other) noexcept:
            signal_(other.signal_), id_(other.id_), persist_mode_(other.persist_mode_) {
            other.signal_.reset();
        }
	    Connection& operator=(const Connection& other) = delete;
	    Connection& operator=(Connection&& other) noexcept {
            signal_ = other.signal_;
            id_ = other.id_;
            persist_mode_ = other.persist_mode_;
            other.signal_.reset();
            return *this;
        }
        Connection(Handler_id id, const std::shared_ptr<Signal> signal, bool persist_mode = false):
           signal_(signal), id_(id), persist_mode_(persist_mode) {}
        Connection(): signal_(nullptr) {}
        ~Connection() {
            if (not persist_mode_) disconnect();
        }
        void disconnect() {
            if (auto ev = signal_.lock()) { ev->disconnect(id_); }
        }
    };
    Connection connect(Handler&& handler, bool persist_connection = false) {
        auto& handlers = signal_->handlers;
        auto id = handlers.insert(handlers.end(), std::move(handler));
        return Connection{id, signal_, persist_connection};
    }
    void disconnect(Handler_id id) {
        signal_->disconnect(id);
    };
    void disconnect_all() {
        signal_->handlers.clear();
    }
    void fire(Params ...args) {
        for (Handler& handler : signal_->handlers) {
            handler(args...);
        }
    }
};
struct Opaque_connection;
struct Opaque_event;
void process_event_stack_entries(int amount = 10);
struct Opaque_signal {
    virtual ~Opaque_signal() = default;
    std::vector<std::unique_ptr<Opaque_connection>> connections;
    void disconnect(Opaque_connection* p);
};
struct Opaque_event {
    virtual ~Opaque_event() = default;
    Opaque_event();
    std::shared_ptr<Opaque_signal> signal;
};
struct Opaque_connection {//has weird semantics, deal with it
    virtual ~Opaque_connection();
    Opaque_connection(const Opaque_connection& other);
    Opaque_connection& operator=(const Opaque_connection& other);
    Opaque_connection(Opaque_connection&& other) noexcept;
    Opaque_connection& operator=(Opaque_connection&& other) noexcept;
    uintptr_t opaque_handler;//casting the function pointer to a uintptr_t as generic address, more platform independent approach compared to void*
    std::weak_ptr<Opaque_signal> signal;
    bool auto_disconnect{true};
    Opaque_connection(const std::shared_ptr<Opaque_signal>& signal, uintptr_t opaque_handler);
    void disconnect();
    virtual void receive(void* data) = 0;
};
namespace intern {
struct Event_stack_entry {
    int32_t pool_address;
    void* data;
};
inline common::Flat_stack<Event_stack_entry> event_stack;
inline common::Lazy_pool<Opaque_connection*> connection_pool;
}/*intern end*/
template <class Data>
struct Connection: public Opaque_connection {
    using Handler = void(*)(const Data&);
    Connection(Handler fn, const std::shared_ptr<Opaque_signal>& signal):
        Opaque_connection(signal, reinterpret_cast<uintptr_t>(fn)) {
    }
    virtual void receive(void* data) override {
        reinterpret_cast<Handler>(opaque_handler)(*static_cast<Data*>(data));
    }
};
template <>
struct Connection<void>: public Opaque_connection {
    using Handler = void(*)();
    Connection(Handler fn, const std::shared_ptr<Opaque_signal>& signal):
        Opaque_connection(signal, reinterpret_cast<uintptr_t>(fn)) {
    }
    virtual void receive(void* data) override {
        common::print("received");
        reinterpret_cast<Handler>(opaque_handler)();
    }
};
template <class Data>
struct Event: public Opaque_event {
    Data data;
    void send() {
        for (auto& connection : signal->connections) {
            const size_t old_capacity = intern::event_stack.capacity();
            intern::event_stack.push({.pool_address = intern::connection_pool.allocate([&connection](Opaque_connection*& p){
                if (p == nullptr) {
                    p = new Connection<Data>{*static_cast<Connection<Data>*>(connection.get())};
                } else {
                    p->~Opaque_connection();
                    new (p) Connection<Data>{*static_cast<Connection<Data>*>(connection.get())};
                }
            }), .data = &data});
        }
    }
    Connection<Data>& connect(const Connection<Data>::Handler& fn) {
        signal->connections.emplace_back(std::make_unique<Connection<Data>>(fn, signal));
        return static_cast<Connection<Data>&>(*signal->connections.back());
    }
};
template <>
struct Event<void>: public Opaque_event {
    void send() {
        for (auto& connection : signal->connections) {
            const size_t old_capacity = intern::event_stack.capacity();
            intern::event_stack.push({.pool_address = intern::connection_pool.allocate([&connection](Opaque_connection*& p){
                if (p == nullptr) {
                    p = new Connection<void>{*static_cast<Connection<void>*>(connection.get())};
                } else {
                    p->~Opaque_connection();
                    new (p) Connection<void>{*static_cast<Connection<void>*>(connection.get())};
                }
            }), .data = nullptr});
        }
    }
    Connection<void>& connect(const Connection<void>::Handler& fn) {
        signal->connections.emplace_back(std::make_unique<Connection<void>>(fn, signal));
        return static_cast<Connection<void>&>(*signal->connections.back());
    }
};
}
