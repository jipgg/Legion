--!strict
local Event = require 'Event'
export type Mouse_button_type = "left" | "right"| "middle"
type Event<params...> = Event.Event<params...>
export type key_event = Event<Keycode>
export type mouse_event = Event<Mouse_button_type, V2>
export type void_event = Event<>
export type update_event = Event<number>
local namespace = {
    updating = Event.construct()::update_event,
    rendering = Event.construct()::void_event,
    shutting_down = Event.construct()::void_event,
    key_pressed = Event.construct()::key_event,
    key_released = Event.construct()::key_event,
    mouse_pressed = Event.construct()::mouse_event,
    mouse_released = Event.construct()::mouse_event,
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
_G["__builtin_mousedown_socket"] = function(button: Mouse_button_type, mouse_pos: V2)
    namespace.mouse_pressed:fire(button, mouse_pos)
end
_G["__builtin_mouseup_socket"] = function(button: Mouse_button_type, mouse_pos: V2)
    namespace.mouse_released:fire(button, mouse_pos)
end
_G["__builtin_keydown_socket"] = function(key: Keycode)
    namespace.key_pressed:fire(key)
end
_G["__builtin_keyup_socket"] = function(key: Keycode)
    namespace.key_released:fire(key)
end
return namespace
