local Event = require 'event'
local Module = {
    onUpdate = Event.new()::Event.Event<number>,
    onRender = Event.new(),
    onShutdown = Event.new(),
}

_G.__builtin_update_socket = function(deltaSeconds: number)
    Module.onUpdate:fire(deltaSeconds)
end
_G.__builtin_render_socket = function()
    Module.onRender:fire()
end
_G.__builtin_shutdown_socket = function()
    Module.onShutdown:fire()
    collectgarbage("collect")
end

return Module
