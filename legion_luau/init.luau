--!strict
local namespace = {}
local Event = require("Event")
export type Event<Args...> = Event.Event<Args...>
export type Event_connection = Event.Connection
namespace.Event = Event

local utitity = require("utility")
export type Pair<Ty1, Ty2> = utitity.Pair<Ty1, Ty2>
namespace.utility = utitity

local state = require("state")
export type Mouse_button = state.Mouse_button
namespace.state = state

return namespace
