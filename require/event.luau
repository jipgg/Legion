--!strict
local Connection = {}
Connection.__index = Connection
function Connection:disconnect()
    self = self::Connection
    local event = self.event
    if event then
        event.connected[self] = nil
    end
end

local function createConnection(event: Event)
    local self = setmetatable({
        event = event
    }, Connection)
    return self
end

local function createWeakTable<K, V>(t: {[K]: V})
    return setmetatable({}::{[K]: V}, {__mode = 'k'})
end

local Event = {}
Event.__index = Event

function Event.new()
    local self = setmetatable({
        connected = createWeakTable({}::{[Connection]: (...any)->()}),
    }, Event)
    return self
end

function Event:connect(fn: (...any)->())
    self = self::Event
    local c = createConnection(self)
    self.connected[c] = fn
    return c
end

function Event.reveal<Ts...>(signal: Signal<Ts...>): Event
    return signal::any
end

function Event:fire(...)
    self = self::Event
    for connection, handler in self.connected do
        handler(...)
    end
end

function Event:signal_cast<Ts...>(): Signal<Ts...>
    return (self::unknown)::Signal<Ts...>
end

export type Connection = typeof(createConnection({}::Event))
export type Event = typeof(Event.new())
export type Signal<Ts...> = {
    connect: (self: unknown, fn: (Ts...)->()) -> Connection
}

return Event
