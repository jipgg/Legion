local Library = {}
local input = require 'input'
export type MouseButtonEvent = input.MouseButtonEvent
export type KeyboardEvent = input.KeyboardEvent
Library.input = input

local run = require 'run'
Library.run = run
local Event = require 'Event'
export type Event<Ts...> = Event.Event<Ts...>
export type EventSignal<Ts...> = Event.EventSignal<Ts...>
export type Connection = Event.Connection
Library.Event = Event

local Buffer = require 'Buffer'
export type Buffer = Buffer.Buffer
export type FixedSizeType = Buffer.Type
Library.Buffer = Buffer

local iterator = require 'iterator'
Library.iterator = iterator

local fs = require 'filesystem'
Library.fs = fs

local sdl = require 'SDL'
Library.sdl = sdl

local trs = require 'TRS'
Library.trs = trs

local Image = require 'Image'
export type Image = Image.Image
Library.Image = Image

return Library
