--!strict
export type Event<Ts...> = {
    _CURR_ID: number,
    _handlers: {[number]: (...unknown)->()},
    destroy: (Event<Ts...>) -> (),
    connect: (Event<Ts...>, (Ts...)->()) -> Connection,
    fire: (Event<Ts...>, Ts...) -> (),
}
export type Connection = {
    _id: number,
    _opaque_event: unknown,
    disconnect: (Connection)->(),
}
--Connection
local Connection: Connection = {}::never
(Connection::never).__index = Connection
function Connection:disconnect()
    ((self._opaque_event)::Event<...unknown>)._handlers[self._id] = nil
end
--Event
local Event: Event<...unknown> = {_CURR_ID = 0}::never 
(Event::never).__index = Event;
function Event:destroy()
    table.clear(self._handlers)
    self = nil::never
end
function Event:fire(...)
    for i, handler in self._handlers do handler(...) end
end
function Event:connect(fn)
    Event._CURR_ID += 1
    local id = Event._CURR_ID
    self._handlers[id] = fn
    return setmetatable({_id = id, _opaque_event = self}::any, Connection)
end
--Constructor
local Constructor = {}
function Constructor.create(): Event<...any>
    return setmetatable({
        _handlers = {},
    }::any, Event)
end
function Constructor.copy<Ts...>(other: Event<Ts...>)
    local this = Constructor.create()
    for i, handler in (other::Event<Ts...>)._handlers do
        this._handlers[i] = handler
    end
    return this
end
return Constructor 
