local Event = require 'Event'
export type KeyboardEvent = Event.Event<Keycode>
export type MouseButtonEvent = Event.Event<MouseButtonType, Vec2>
export type KeyboardSignal = Event.EventSignal<Keycode>
export type MouseButtonSignal = Event.EventSignal<MouseButtonType, Vec2>
local Module = {
    onKeyPressed = Event.new()::KeyboardEvent,
    onKeyReleased = Event.new()::KeyboardEvent,
    onMousePressed = Event.new()::MouseButtonEvent,
    onMouseReleased = Event.new()::MouseButtonEvent,
    isKeyDown = cimport('isKeyDown'),
    getMousePosition = cimport('getMousePosition')
}
_G.__builtin_keydown_socket = function(keycode: Keycode)
    Module.onKeyPressed:fire(keycode)
end
_G.__builtin_keyup_socket = function(keycode: Keycode)
    Module.onKeyReleased:fire(keycode)
end
_G.__builtin_mousedown_socket = function(button: MouseButtonType, pos: Vec2)
    Module.onMousePressed:fire(button, pos)
end
_G.__builtin_mouseup_socket = function(button: MouseButtonType, pos: Vec2)
    Module.onMouseReleased:fire(button, pos)
end

return Module
