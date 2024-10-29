--!strict
type _handler<params...> = (params...) -> ()
type _event<params...> = {
    _handlers: {_handler<params...>},
    destroy: (event<params...>) -> (),
    connect: (event<params...>, (params...)->()) -> connection,
    fire: (event<params...>, params...) -> (),
};
type _connection = {
    _id: number,
    _opaque_event: unknown,
    disconnect: (connection)->(),
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
export type event<params...> = typeof(setmetatable({}::_event<params...>, event))
export type connection = typeof(setmetatable({}::_connection, connection))
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
    export type constructor = <params...>() -> event<params...>
function construct<params...>(): event<params...>
    return setmetatable({
        _handlers = {},
    }, event)::event<params...>
end
return construct
