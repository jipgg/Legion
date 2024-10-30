--!strict
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
local event = {
    CURR_ID = 0,
    __type = 'event',
} 
event.__index = event
export type Event<Params...> = typeof(setmetatable({}::_Event<Params...>, event))
export type Connection = typeof(setmetatable({}::_Connection, connection))
function event:destroy()
    table.clear(self._handlers)
end
function event:fire(...)
    for i, handler in self._handlers do handler(...) end
end
function event:connect(fn)
    event.CURR_ID += 1
    local id = event.CURR_ID
    self._handlers[id] = fn
    local c = {_id = id, _opaque_event = self}
    return setmetatable(c, connection)
end
    --Constructor
local class = setmetatable({}, {__call = function<Params...>(self): Event<Params...>
    return self.construct()
end})
function class.construct<params...>(): Event<params...>
    return setmetatable({
        _handlers = {},
    }, event)::Event<params...>
end
return class
