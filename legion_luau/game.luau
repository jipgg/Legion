local Event = require "Event"
type Event<Ts...> = Event.Event<Ts...>
local UPDATE: "__legion_update_handler" = "__legion_update_handler"
local RENDER: "__legion_render_handler" = "__legion_render_handler"
local START: "__legion_start_handler" = "__legion_start_handler"
local SHUTDOWN: "__legion_shutdown_handler" = "__legion_shutdown_handler"
local MOUSE_BUTTON_UP: "__legion_mouse_button_up_handler" = "__legion_mouse_button_up_handler"
local MOUSE_BUTTON_DOWN: "__legion_mouse_button_down_handler" = "__legion_mouse_button_down_handler"
local KEY_DOWN: "__legion_key_down" = "__legion_key_down"
export type Mouse_button = "Left" | "Middle" | "Right"
local namespace = {
    render = Event.create()::Event<>,
    key_down = Event.create()::Event<Key>,
}
local mouse = {
    button_down = Event.create()::Event<Mouse_button, Vec2>,
    button_up = Event.create()::Event<Mouse_button, Vec2>
}
namespace.mouse = mouse
local builtin = {}
namespace._builtin = builtin

function builtin.update_fn(fn: (number)->()): ()
    _G[UPDATE] = fn
end
function builtin.render_fn(fn: ()->()): ()
    _G[RENDER] = fn
end
function builtin.start_fn(fn: ()->()): ()
    _G[START] = fn
end
function builtin.shutdown_fn(fn: ()->()): ()
    _G[SHUTDOWN] = fn
end
function builtin.mouse_button_up_fn(fn: (Mouse_button, Vec2)->())
    _G[MOUSE_BUTTON_UP] = fn
end
function builtin.mouse_button_down_fn(fn: (Mouse_button, Vec2)->())
    _G[MOUSE_BUTTON_DOWN] = fn
end
function builtin.key_down_fn(fn: (Key) -> ())
    _G[KEY_DOWN] = fn
end
builtin.render_fn(function()
    namespace.render:fire()
end)
builtin.mouse_button_down_fn(function(type: Mouse_button, pos: Vec2)
    mouse.button_down:fire(type, pos)
end)
builtin.mouse_button_up_fn(function(type: Mouse_button, pos: Vec2)
    mouse.button_up:fire(type, pos)
end)
builtin.key_down_fn(function(key: Key)
    namespace.key_down:fire(key)
end)
return namespace
