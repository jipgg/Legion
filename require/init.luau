local standard_library = {}
local input = require 'input'
export type MouseButtonEvent = input.MouseButtonEvent
export type KeyboardEvent = input.KeyboardEvent
standard_library.input = input

local run = require 'run'
standard_library.run = run
local Event = require 'Event'
export type Event<Ts...> = Event.Event<Ts...>
export type EventSignal<Ts...> = Event.EventSignal<Ts...>
export type Connection = Event.Connection
standard_library.Event = Event

local Buffer = require 'Buffer'
export type Buffer = Buffer.Buffer
export type FixedSizeType = Buffer.Type
standard_library.Buffer = Buffer

local iterator = require 'iterator'
standard_library.iterator = iterator

local fs = require 'filesystem'
standard_library.fs = fs

local sdl = require 'SDL'
standard_library.sdl = sdl

local trs = require 'TRS'
standard_library.trs = trs

return standard_library
