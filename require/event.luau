--!strict
export type event<params...> = {
    _CURR_ID: number,
    _handlers: table<(params...)->()>,
    destroy: (event<params...>) -> (),
    connect: (event<params...>, (params...)->()) -> connection,
    fire: (event<params...>, params...) -> (),
    __index: event<params...>,
    __type: "event",
};
export type connection = {
    _id: number,
    _opaque_event: unknown,
    disconnect: (connection)->(),
    __index: connection,
    __mode: 'v',
    __type: 'connection',
};
local connection: connection = {
    __type = 'connection',
    __mode = 'v',
}::connection
connection.__index = connection
function connection:disconnect()
    if self._opaque_event ~= nil then
        local event = self._opaque_event::event<...unknown>
        event._handlers[self._id] = nil
    end
end
local event: event<...unknown> = {
    _CURR_ID = 0,
    __type = 'event',
}::event<...unknown> 
event.__index = event
function event:destroy()
    table.clear(self._handlers)
end
function event:fire(...)
    for i, handler in self._handlers do handler(...) end
end
function event:connect(fn)
    event._CURR_ID += 1
    local id = event._CURR_ID
    self._handlers[id] = fn
    local connection: connection = {_id = id, _opaque_event = self}::connection
    return setmetatable(connection, connection)::any
end
--Constructor
function construct(): event<...any>
    local self: event<...unknown> = {
        _handlers = {},
    }::event<...unknown>
    return setmetatable(self, event)::any
end
return construct
