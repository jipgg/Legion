--!strict
--local Event = require 'Event'
export type MouseButtonType = "left" | "right"| "middle"
--type Event<params...> = Event.Event<params...>
local Module = {
    updating = Event(),
    rendering = Event(),
    shuttingDown = Event(),
    keyPressed = Event(),
    keyReleased = Event(),
    mousePressed = Event(),
    mouseReleased = Event(),
}
_G["__builtin_update_socket"] = function(deltaSeconds: number)
    Module.updating:fire(deltaSeconds)
    collectgarbage("collect")
end
_G["__builtin_render_socket"] = function()
    Module.rendering:fire()
end
_G["__builtin_shutdown_socket"] = function()
    Module.shuttingDown:fire()
    collectgarbage("collect")
end
_G["__builtin_mousedown_socket"] = function(button: MouseButtonType, mousePos: V2)
    Module.mousePressed:fire(button, mousePos)
end
_G["__builtin_mouseup_socket"] = function(button: MouseButtonType, mousePos: V2)
    Module.mouseReleased:fire(button, mousePos)
end
_G["__builtin_keydown_socket"] = function(key: Keycode)
    Module.keyPressed:fire(key)
end
_G["__builtin_keyup_socket"] = function(key: Keycode)
    Module.keyReleased:fire(key)
end
return Module
