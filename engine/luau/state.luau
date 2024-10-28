--!strict
local Event = require 'Event'
type Event<Ts...> = Event.Event<Ts...>
local namespace = {
    updating = Event.create()::Event<number>,
    rendering = Event.create()::Event<>,
    shutting_down = Event.create()::Event<>,
    key_pressed = Event.create()::Event<keycode>,
    key_released = Event.create()::Event<keycode>,
    mouse_pressed = Event.create()::Event<mouse_button_type, vec2d>,
    mouse_released = Event.create()::Event<mouse_button_type, vec2d>,
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
