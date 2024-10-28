--!strict
local event = require 'event'
type event<params...> = event.event<params...>
local namespace = {
    updating = event()::event<number>,
    rendering = event()::event<>,
    shutting_down = event()::event<>,
    key_pressed = event()::event<keycode>,
    key_released = event()::event<keycode>,
    mouse_pressed = event()::event<mouse_button_type, vec2d>,
    mouse_released = event()::event<mouse_button_type, vec2d>,
}
_G["__builtin_update_socket"] = function(delta_s: number)
    namespace.updating:fire(delta_s)
    collectgarbage("collect")
end
_G["__builtin_render_socket"] = function()
    namespace.rendering:fire()
end
_G["__builtin_shutdown_socket"] = function()
    namespace.shutting_down:fire()
    collectgarbage("collect")
end
_G["__builtin_mousedown_socket"] = function(button: mouse_button_type, mouse_pos: vec2d)
    namespace.mouse_pressed:fire(button, mouse_pos)
end
_G["__builtin_mouseup_socket"] = function(button: mouse_button_type, mouse_pos: vec2d)
    namespace.mouse_released:fire(button, mouse_pos)
end
_G["__builtin_keydown_socket"] = function(key: keycode)
    namespace.key_pressed:fire(key)
end
_G["__builtin_keyup_socket"] = function(key: keycode)
    namespace.key_released:fire(key)
end
return namespace
