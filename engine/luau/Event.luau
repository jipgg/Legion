--!strict
export type Event<Ts...> = {
    _CURR_ID: number,
    _handlers: table<(Ts...)->()>,
    destroy: (Event<Ts...>) -> (),
    connect: (Event<Ts...>, (Ts...)->()) -> Connection,
    fire: (Event<Ts...>, Ts...) -> (),
    __index: Event<Ts...>,
    __type: "Event",
};
export type Connection = {
    _id: number,
    _opaque_event: unknown,
    disconnect: (Connection)->(),
    __index: Connection,
    __mode: 'v',
    __type: 'Connection',
};
local Connection: Connection = {
    __type = 'Connection',
    __mode = 'v',
}::Connection
Connection.__index = Connection
function Connection:disconnect()
    if self._opaque_event ~= nil then
        local event = self._opaque_event::Event<...unknown>
        event._handlers[self._id] = nil
    end
end
local Event: Event<...unknown> = {
    _CURR_ID = 0,
    __type = 'Event',
}::Event<...unknown> 
Event.__index = Event
function Event:destroy()
    table.clear(self._handlers)
end
function Event:fire(...)
    for i, handler in self._handlers do handler(...) end
end
function Event:connect(fn)
    Event._CURR_ID += 1
    local id = Event._CURR_ID
    self._handlers[id] = fn
    local connection: Connection = {_id = id, _opaque_event = self}::Connection
    return setmetatable(connection, Connection)::any
end
--Constructor
local class = {}
-- Creates a new event object
function class.create(): Event<...any>
    local self: Event<...unknown> = {
        _handlers = {},
    }::Event<...unknown>
    return setmetatable(self, Event)::any
end
function class.copy<Ts...>(other: Event<Ts...>)
    local this = class.create()
    for i, handler in (other::Event<Ts...>)._handlers do
        this._handlers[i] = handler
    end
    return this
end
return class
