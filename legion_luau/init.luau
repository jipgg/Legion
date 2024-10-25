--!strict
local namespace = {}
local Event = require("Event")
export type Event<Args...> = Event.Event<Args...>
export type Event_connection = Event.Connection
namespace.Event = Event

local utitity = require("utility")
export type Pair<Ty1, Ty2> = utitity.Pair<Ty1, Ty2>
namespace.utility = utitity
local game = require("game")
export type Mouse_button = game.Mouse_button
namespace.game = game

return namespace
