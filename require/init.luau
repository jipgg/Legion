--!strict
local namespace = {}

local event = require 'event'
export type event<params...> = event.event<params...>
export type connection = event.connection
namespace.event = event

local state = require 'state'
namespace.state = state

local button = require 'button'
export type Button = button.Button
namespace.button = button

local filesystem = require 'filesystem'
export type Path = filesystem.Path
export type Directory_entry = filesystem.Directory_entry
export type perms = filesystem.perms
export type copy_options = filesystem.copy_options
export type file_type = filesystem.file_type
namespace.filesystem = filesystem

return namespace
