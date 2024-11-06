--!strict
export type EventSignal<Params...> = {
    connect: (self: EventSignal<Params...>, (Params...)->()) -> Connection,
}
type _Handler<Params...> = (Params...) -> ()
type _Event<Params...> = {
    _handlers: {_Handler<Params...>},
    destroy: (Event<Params...>) -> (),
    connect: (Event<Params...>, (Params...)->()) -> Connection,
    fire: (Event<Params...>, Params...) -> (),
};
type _Connection = {
    _id: number,
    _opaque_event: unknown,
    disconnect: (Connection)->(),
};
local connection = {
    __type = 'connection',
    __mode = 'v',
}
connection.__index = connection
function connection:disconnect()
    if self._opaque_event ~= nil then
        local event = self._opaque_event
        event.handlers[self.id] = nil
    end
end
local Event = {
    CURR_ID = 0,
    __type = 'event',
} 
Event.__index = Event
export type Event<Params...> = typeof(setmetatable({}::_Event<Params...>, Event))
export type Connection = typeof(setmetatable({}::_Connection, connection))
function Event:destroy()
    table.clear(self._handlers)
end
function Event:fire(...)
    for i, handler in self._handlers do handler(...) end
end
function Event:connect(fn)
    Event.CURR_ID += 1
    local id = Event.CURR_ID
    self._handlers[id] = fn
    local c = {_id = id, _opaque_event = self}
    return setmetatable(c, connection)
end
function Event.new<params...>(): Event<params...>
    return setmetatable({
        _handlers = {},
    }, Event)::Event<params...>
end
return Event
