#include "event.h"
namespace event {
void process_event_stack_entries(int amount) {
    while(not intern::event_stack.empty() and --amount >= 0) {
        auto& [address, data] = intern::event_stack.top();
        opaque_connection* connection = intern::connection_pool.at(address);
        if (not connection->signal.expired()) {
            connection->receive(data);
        }
        intern::connection_pool.free(address);
        intern::event_stack.pop();
    }
}
opaque_event::opaque_event(): signal(std::make_shared<opaque_signal>()) {
}
opaque_connection::~opaque_connection() {
    if (auto_disconnect) disconnect();
}
opaque_connection::opaque_connection(const opaque_connection& a): signal(a.signal), opaque_handler(a.opaque_handler), auto_disconnect(false) {
}
opaque_connection& opaque_connection::operator=(const opaque_connection& a) {
    signal = a.signal;
    opaque_handler = a.opaque_handler;
    auto_disconnect = false;
    return *this;
}
opaque_connection::opaque_connection(opaque_connection&& a) noexcept: signal(a.signal), opaque_handler(a.opaque_handler) {
    a.auto_disconnect = false;
}
opaque_connection& opaque_connection::operator=(opaque_connection&& a) noexcept {
    signal = a.signal;
    opaque_handler = a.opaque_handler;
    a.auto_disconnect = false;
    return *this;
}
opaque_connection::opaque_connection(const std::shared_ptr<opaque_signal>& signal, uintptr_t opaque_handler):
    signal(signal),
    opaque_handler(opaque_handler) {
}
void opaque_connection::disconnect() {
    if (auto e = signal.lock()) {
        e->disconnect(this);
    }
}

void opaque_signal::disconnect(opaque_connection* p) {
    auto it = std::find_if(
        connections.begin(),
        connections.end(),
        [&p](auto& e){return e.get() == p;}
    );
    if (it != connections.end()) {
        connections.back().swap(*it);
        connections.pop_back();
    } else {
        common::printerr("connection has already been destroyed.");
    }
}
}
