#include "event.h"
namespace event {
void process_event_stack_entries(int amount) {
    while(not intern::event_stack.empty() and --amount >= 0) {
        auto& [address, data] = intern::event_stack.top();
        Opaque_connection* connection = intern::connection_pool.at(address);
        if (not connection->signal.expired()) {
            connection->receive(data);
        }
        intern::connection_pool.free(address);
        intern::event_stack.pop();
    }
}
Opaque_event::Opaque_event(): signal(std::make_shared<Opaque_signal>()) {
}
Opaque_connection::~Opaque_connection() {
    if (auto_disconnect) disconnect();
}
Opaque_connection::Opaque_connection(const Opaque_connection& a): signal(a.signal), opaque_handler(a.opaque_handler), auto_disconnect(false) {
}
Opaque_connection& Opaque_connection::operator=(const Opaque_connection& a) {
    signal = a.signal;
    opaque_handler = a.opaque_handler;
    auto_disconnect = false;
    return *this;
}
Opaque_connection::Opaque_connection(Opaque_connection&& a) noexcept: signal(a.signal), opaque_handler(a.opaque_handler) {
    a.auto_disconnect = false;
}
Opaque_connection& Opaque_connection::operator=(Opaque_connection&& a) noexcept {
    signal = a.signal;
    opaque_handler = a.opaque_handler;
    a.auto_disconnect = false;
    return *this;
}
Opaque_connection::Opaque_connection(const std::shared_ptr<Opaque_signal>& signal, uintptr_t opaque_handler):
    signal(signal),
    opaque_handler(opaque_handler) {
}
void Opaque_connection::disconnect() {
    if (auto e = signal.lock()) {
        e->disconnect(this);
    }
}

void Opaque_signal::disconnect(Opaque_connection* p) {
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
