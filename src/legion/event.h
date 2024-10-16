#pragma once
#include "common.h"
#include "Lazy_pool.h"
#include <cstdint>
#include <vector>
#include <memory>
namespace event {
using Event_id = std::uint32_t;
using Connection_id = std::uint32_t;
namespace base {
struct Connection;
struct Event;
struct Signal;
}
struct base::Signal {
    virtual ~Signal() = default;
    void send(void* data);
    std::vector<std::unique_ptr<base::Connection>> connections_;
    void disconnect(base::Connection* p) {
        auto it = std::find_if(
            connections_.begin(),
            connections_.end(),
            [&p](auto& e){return e.get() == p;}
        );
        if (it != connections_.end()) {
            connections_.back().swap(*it);
            connections_.pop_back();
        } else {
            printerr("connection has already been destroyed.");
        }
    }
};
struct base::Event {
    virtual ~Event() = default;
    Event(): signal_(std::make_shared<base::Signal>()) {}
    std::shared_ptr<base::Signal> signal_;
};
struct base::Connection {
    void* opaque_handler_;
    std::weak_ptr<base::Signal> signal_;
    Connection(const std::shared_ptr<base::Signal>& signal, void* opaque_handler): signal_(signal), opaque_handler_(opaque_handler) {}
    void disconnect() {
        if (auto e = signal_.lock()) {
            e->disconnect(this);
        }
    }
    virtual ~Connection() = default;
    virtual void receive(void* data) = 0;
};
//need to make an object pool
struct Event_stack_entry {
    int32_t pool_address;
    void* data;
};
static Flat_stack<Event_stack_entry> event_stack;
static Lazy_pool<base::Connection*> connection_pool;
inline void base::Signal::send(void* data) {
    for (auto& a : connections_) {
    }
}
template <class Data>
class Connection: public base::Connection {
public:
    using Handler = void(*)(const Data&);
    Connection(Handler fn, const std::shared_ptr<base::Signal>& ev): base::Connection(ev, fn) {}
    virtual void receive(void* data) override {
        print("received");
        static_cast<Handler>(opaque_handler_)(*static_cast<Data*>(data));
    }
private:
};
template <class Data>
struct Event: public base::Event {
    Data data;
    void fire() {
        for (auto& connection : signal_->connections_) {
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
        signal_->connections_.emplace_back(std::make_unique<Connection<Data>>(fn, signal_));
        return static_cast<Connection<Data>&>(*signal_->connections_.back());
    }
};
inline void handle_event_stack(int max_per_frame = 10) {
    while(not event_stack.empty() and --max_per_frame >= 0) {
        auto& [address, data] = event_stack.top();
        base::Connection* connection = connection_pool.at(address);
        if (not connection->signal_.expired()) {
            connection->receive(data);
        }
        connection_pool.free(address);
        event_stack.pop();
    }
}
}//event
