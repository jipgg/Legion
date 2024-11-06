local Event = require 'Event'
export type KeyboardSignal = Event.Signal<Keycode>
export type MouseButtonSignal = Event.Signal<MouseButtonType, Vec2>
local Module = {
    onKeyPressed = Event.new():signal_cast()::KeyboardSignal,
    onKeyReleased = Event.new():signal_cast()::KeyboardSignal,
    onMousePressed = Event.new():signal_cast()::MouseButtonSignal,
    onMouseReleased = Event.new():signal_cast()::MouseButtonSignal,
    isKeyDown = cimport('isKeyDown'),
    getMousePosition = cimport('getMousePosition')
}
_G.__builtin_keydown_socket = function(keycode: Keycode)
    Event.reveal(Module.onKeyPressed):fire(keycode)
end
_G.__builtin_keyup_socket = function(keycode: Keycode)
    Event.reveal(Module.onKeyReleased):fire(keycode)
end
_G.__builtin_mousedown_socket = function(button: MouseButtonType, pos: Vec2)
    Event.reveal(Module.onMousePressed):fire(button, pos)
end
_G.__builtin_mouseup_socket = function(button: MouseButtonType, pos: Vec2)
    Event.reveal(Module.onMouseReleased):fire(button, pos)
end

return Module
