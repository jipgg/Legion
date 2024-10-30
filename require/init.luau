--!strict
local namespace = {}

local Event = require 'Event'
export type Event<Params...> = event.Event<Params...>
export type Connection = event.Connection
namespace.Event = Event

local state = require 'state'
namespace.state = state

return namespace
