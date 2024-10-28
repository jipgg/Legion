--!strict
local namespace = {}

local event = require 'event'
export type event<params...> = event.event<params...>
export type event_connection = event.connection
namespace.event = event

local state = require 'state'
namespace.state = state

local button = require 'button'
export type button = button.button
namespace.button = button

return namespace
