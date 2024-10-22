#include "legion/event.h"
namespace legion {
void process_pushed_events(int amount) {
    while(not intern::event_stack.empty() and --amount >= 0) {
        auto& [address, data] = intern::event_stack.top();
        intern::Connection* connection = intern::connection_pool.at(address);
        if (not connection->signal.expired()) {
            connection->receive(data);
        }
        intern::connection_pool.free(address);
        intern::event_stack.pop();
    }
}
namespace intern {
Event::Event(): signal(std::make_shared<Signal>()) {
}
Connection::~Connection() {
    disconnect();
}
Connection::Connection(Connection&& other) noexcept:
    signal(other.signal),
    opaque_handler(other.opaque_handler) {
    other.signal.reset();
}
Connection& Connection::operator=(Connection&& other) noexcept {
    signal.swap(other.signal);
    opaque_handler = other.opaque_handler;
    return *this;
}
Connection::Connection(const std::shared_ptr<Signal>& signal, uintptr_t opaque_handler):
    signal(signal),
    opaque_handler(opaque_handler) {
}
void Connection::disconnect() {
    if (auto e = signal.lock()) {
        e->disconnect(this);
    }
}

void Signal::disconnect(Connection* p) {
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
}//intern end
}//legion end
