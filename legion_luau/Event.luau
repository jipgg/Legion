--!strict
export type Event<Ts...> = {
    _CURR_ID: number,
    _handlers: {[number]: (Ts...)->()},
    destroy: (Event<Ts...>) -> (),
    connect: (Event<Ts...>, (Ts...)->()) -> Connection,
    fire: (Event<Ts...>, Ts...) -> (),
    __index: Event<Ts...>,
};
export type Connection = {
    _id: number,
    _opaque_event: unknown,
    disconnect: (Connection)->(),
    __index: Connection,
    __mode: Weak_v,
};
local Connection: Connection = {}::never
Connection.__index = Connection
Connection.__mode = 'v'
function Connection:disconnect()
    if self._opaque_event ~= nil then
        local event = self._opaque_event::Event<...unknown>
        event._handlers[self._id] = nil
    end
end
local Event: Event<...unknown> = {_CURR_ID = 0}::never 
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
    return setmetatable({_id = id, _opaque_event = self}::any, Connection)
end
--Constructor
local class = {}
-- Creates a new event object
function class.create(): Event<...any>
    return setmetatable({
        _handlers = {},
    }::any, Event)
end
function class.copy<Ts...>(other: Event<Ts...>)
    local this = class.create()
    for i, handler in (other::Event<Ts...>)._handlers do
        this._handlers[i] = handler
    end
    return this
end
return class
