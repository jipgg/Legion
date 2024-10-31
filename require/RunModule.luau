local Module = {
    onUpdate = Event(),
    onRender = Event(),
    onShutdown = Event(),
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
