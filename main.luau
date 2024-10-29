--!strict
local lg = require '@Legion'
local state = lg.state
local dummy = physical_component()
dummy.velocity = vec2d(100, 100)
local a = dummy:bounds();
print(dummy.velocity:unit())
print(dummy.size.x)
print(a)
print(builtin.title, builtin.window_size)
local rect = recti64(100, 100, 200, 100)

local resources = fs.path("resources");
local jw = resources / "jw.png"
if fs.exists(jw) then
    local text, w, h = sdl.texture(tostring(jw))
    if text then
        print(typeof(text), w, h)
        print("ok")
        state.rendering:connect(function()
            renderer.render(text, recti64(0, 0, w, h))
        end)
    else
        print("not ok")
    end
end

state.key_pressed:connect(function(key: keycode)
    if key == "a" then
        local mouse_pos = builtin.mouse_pos()
        if builtin.point_in_rect(mouse_pos, rect) then
            print("Yes")
        end
    end
end)
state.key_released:connect(function(key: keycode)
    if key == "a" then
        print("up")
    end
end)
state.mouse_pressed:connect(function(mouse: mouse_button_type, pos: vec2d)
end)
