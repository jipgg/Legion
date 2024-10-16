#pragma once
#include "common.h"
#include "Lazy_pool.h"
#include <cstdint>
#include <vector>
#include <memory>
namespace event {
namespace intern {
struct Connection;
struct Event;
struct Event_stack_entry {
    int32_t pool_address;
    void* data;
};
void process_pushed_events(int amount = 10);
inline Flat_stack<Event_stack_entry> event_stack;
inline Lazy_pool<Connection*> connection_pool;
struct Signal {
    virtual ~Signal() = default;
    std::vector<std::unique_ptr<Connection>> connections;
    void disconnect(Connection* p);
};
struct Event {
    virtual ~Event() = default;
    Event();
    std::shared_ptr<Signal> signal;
};
struct Connection {
    virtual ~Connection() = default;//maybe add raii after having customized the copy and move semantics
    uintptr_t opaque_handler;//casting the function pointer to a uintptr_t as generic address, more platform independent approach compared to void*
    std::weak_ptr<Signal> signal;
    Connection(const std::shared_ptr<Signal>& signal, uintptr_t opaque_handler);
    void disconnect();
    virtual void receive(void* data) = 0;
};
}/*intern end*/

template <class Data>
struct Connection: public intern::Connection {
    using Handler = void(*)(const Data&);
    Connection(Handler fn, const std::shared_ptr<intern::Signal>& signal): intern::Connection(signal, reinterpret_cast<uintptr_t>(fn)) {}
    virtual void receive(void* data) override {
        print("received");
        reinterpret_cast<Handler>(opaque_handler)(*static_cast<Data*>(data));
    }
};
template <>
struct Connection<void>: public intern::Connection {
    using Handler = void(*)();
    Connection(Handler fn, const std::shared_ptr<intern::Signal>& signal): intern::Connection(signal, reinterpret_cast<uintptr_t>(fn)) {}
    virtual void receive(void* data) override {
        print("received");
        reinterpret_cast<Handler>(opaque_handler)();
    }
};
template <class Data>
struct Event: public intern::Event {
    Data data;
    void send() {
        for (auto& connection : signal->connections) {
            const size_t old_capacity = intern::event_stack.capacity();
            intern::event_stack.push({.pool_address = intern::connection_pool.allocate([&connection](intern::Connection*& p){
                if (p == nullptr) {
                    p = new Connection<Data>{*static_cast<Connection<Data>*>(connection.get())};
                } else {
                    p->~Connection();
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
struct Event<void>: public intern::Event {
    void send() {
        for (auto& connection : signal->connections) {
            const size_t old_capacity = intern::event_stack.capacity();
            intern::event_stack.push({.pool_address = intern::connection_pool.allocate([&connection](intern::Connection*& p){
                if (p == nullptr) {
                    p = new Connection<void>{*static_cast<Connection<void>*>(connection.get())};
                } else {
                    p->~Connection();
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
