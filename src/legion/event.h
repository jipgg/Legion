#pragma once
#include "common.h"
#include "Lazy_pool.h"
#include <cstdint>
#include <vector>
#include <memory>
namespace event {
namespace base {struct Connection; struct Event;}//forward declarations
struct Event_stack_entry {
    int32_t pool_address;
    void* data;
};
static Flat_stack<Event_stack_entry> event_stack;
static Lazy_pool<base::Connection*> connection_pool;

struct Signal {
    virtual ~Signal() = default;
    std::vector<std::unique_ptr<base::Connection>> connections;
    void disconnect(base::Connection* p) {
        auto it = std::find_if(
            connections.begin(),
            connections.end(),
            [&p](auto& e){return e.get() == p;}
        );
        if (it != connections.end()) {
            connections.back().swap(*it);
            connections.pop_back();
        } else {
            printerr("connection has already been destroyed.");
        }
    }
};
struct base::Event {
    virtual ~Event() = default;
    Event(): signal(std::make_shared<Signal>()) {}
    std::shared_ptr<Signal> signal;
};
struct base::Connection {
    virtual ~Connection() = default;//maybe add raii after having customized the copy and move semantics
    uintptr_t opaque_handler;//casting the function pointer to a uintptr_t as generic address, more platform independent approach compared to void*
    std::weak_ptr<Signal> signal;
    Connection(const std::shared_ptr<Signal>& signal, uintptr_t opaque_handler):
        signal(signal),
        opaque_handler(opaque_handler) {}
    void disconnect() {
        if (auto e = signal.lock()) {
            e->disconnect(this);
        }
    }
    virtual void receive(void* data) = 0;
};
template <class Data>
struct Connection: public base::Connection {
    using Handler = void(*)(const Data&);
    Connection(Handler fn, const std::shared_ptr<Signal>& signal): base::Connection(signal, reinterpret_cast<uintptr_t>(fn)) {}
    virtual void receive(void* data) override {
        print("received");
        reinterpret_cast<Handler>(opaque_handler)(*static_cast<Data*>(data));
    }
};
template <>
struct Connection<void>: public base::Connection {
    using Handler = void(*)();
    Connection(Handler fn, const std::shared_ptr<Signal>& signal): base::Connection(signal, reinterpret_cast<uintptr_t>(fn)) {}
    virtual void receive(void* data) override {
        print("received");
        reinterpret_cast<Handler>(opaque_handler)();
    }
};
template <class Data>
struct Event: public base::Event {
    Data data;
    void send() {
        for (auto& connection : signal->connections) {
            const size_t old_capacity = event_stack.capacity();
            event_stack.push({.pool_address = connection_pool.allocate([&connection](base::Connection*& p){
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
struct Event<void>: public base::Event {
    void send() {
        for (auto& connection : signal->connections) {
            const size_t old_capacity = event_stack.capacity();
            event_stack.push({.pool_address = connection_pool.allocate([&connection](base::Connection*& p){
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
inline void handle_event_stack(int max_per_frame = 10) {
    while(not event_stack.empty() and --max_per_frame >= 0) {
        auto& [address, data] = event_stack.top();
        base::Connection* connection = connection_pool.at(address);
        if (not connection->signal.expired()) {
            connection->receive(data);
        }
        connection_pool.free(address);
        event_stack.pop();
    }
}
}//event
