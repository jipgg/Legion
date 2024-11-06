local Event = require 'event'
local run_library = {
    onUpdate = Event(),
    onRender = Event(),
    onShutdown = Event(),
}

_G.__builtin_update_socket = function(deltaSeconds: number)
    run_library.onUpdate:fire(deltaSeconds)
end
_G.__builtin_render_socket = function()
    run_library.onRender:fire()
end
_G.__builtin_shutdown_socket = function()
    run_library.onShutdown:fire()
    collectgarbage("collect")
end

return run_library
