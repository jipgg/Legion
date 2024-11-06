local Library = {}
local input = require 'input'
Library.input = input

local run = require 'run'
Library.run = run
local Event = require 'Event'
export type Event = Event.Event
export type Signal<Ts...> = Event.Signal<Ts...>
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
