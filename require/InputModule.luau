local Module = {
    onKeyPressed = Event(),
    onKeyReleased = Event(),
    onMousePressed = Event(),
    onMouseReleased = Event(),
    isKeyDown = nil::unknown,
}
_G.__builtin_keydown_socket = function(keycode: Keycode)
    Module.onKeyPressed:fire(keycode)
end
_G.__builtin_keyup_socket = function(keycode: Keycode)
    Module.onKeyReleased:fire(keycode)
end
_G.__builtin_mousedown_socket = function(button: MouseButtonType, pos: V2)
    Module.onMousePressed:fire(button, pos)
end
_G.__builtin_mouseup_socket = function(button: MouseButtonType, pos: V2)
    Module.onMouseReleased:fire(button, pos)
end

return Module
