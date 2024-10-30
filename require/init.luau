--!strict
local Module = {}

local Event = require 'Event'
export type Event<Params...> = event.Event<Params...>
export type Connection = event.Connection
Module.Event = Event

local state = require 'state'
Module.state = state

return Module
