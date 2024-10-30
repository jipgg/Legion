--!strict
local Event = require 'Event'
export type MouseButtonType = "left" | "right"| "middle"
type Event<params...> = Event.Event<params...>
export type KeyboardEvent = Event<Keycode>
export type MouseEvent = Event<MouseButtonType, V2>
export type VoidEvent = Event<>
export type UpdateEvent = Event<number>
local Module = {
    updating = Event.construct()::UpdateEvent,
    rendering = Event.construct()::VoidEvent,
    shuttingDown = Event.construct()::VoidEvent,
    keyPressed = Event.construct()::KeyboardEvent,
    keyReleased = Event.construct()::KeyboardEvent,
    mousePressed = Event.construct()::MouseEvent,
    mouseReleased = Event.construct()::MouseEvent,
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
