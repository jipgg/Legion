--!strict
local namespace = {}

local Event = require 'Event'
export type Event<Args...> = Event.Event<Args...>
export type Event_connection = Event.Connection
namespace.Event = Event

local state = require 'state'
namespace.state = state

local Button = require 'Button'
export type Button = Button.Button
namespace.Button = Button

return namespace
