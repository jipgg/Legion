--!strict
local event = require 'event'
type event<params...> = event.event<params...>
export type key_event = event<keycode>
export type mouse_event = event<mouse_button_type, Vec2d>
export type void_event = event<>
export type update_event = event<number>
local namespace = {
    updating = event()::update_event,
    rendering = event()::void_event,
    shutting_down = event()::void_event,
    key_pressed = event()::key_event,
    key_released = event()::key_event,
    mouse_pressed = event()::mouse_event,
    mouse_released = event()::mouse_event,
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
_G["__builtin_mousedown_socket"] = function(button: mouse_button_type, mouse_pos: Vec2d)
    namespace.mouse_pressed:fire(button, mouse_pos)
end
_G["__builtin_mouseup_socket"] = function(button: mouse_button_type, mouse_pos: Vec2d)
    namespace.mouse_released:fire(button, mouse_pos)
end
_G["__builtin_keydown_socket"] = function(key: keycode)
    namespace.key_pressed:fire(key)
end
_G["__builtin_keyup_socket"] = function(key: keycode)
    namespace.key_released:fire(key)
end
return namespace
