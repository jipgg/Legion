local Event = require 'event'
local Module = {
    onUpdate = Event.new():signal_cast()::Event.Signal<number>,
    onRender = Event.new():signal_cast()::Event.Signal<>,
    onShutdown = Event.new():signal_cast()::Event.Signal<>,
}

_G.__builtin_update_socket = function(deltaSeconds: number)
    Event.reveal(Module.onUpdate):fire(deltaSeconds)
end
_G.__builtin_render_socket = function()
    Event.reveal(Module.onRender):fire()
end
_G.__builtin_shutdown_socket = function()
    Event.reveal(Module.onShutdown):fire()
    collectgarbage("collect")
end

return Module
