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

local main_file = fs.path("main.luau")
local my_path = "test" / main_file
print(`my_path = {my_path}`)
print(`extension {my_path:parent_path()}`)
print(my_path:root_path())
print(my_path:has_stem(), my_path:stem(), my_path:filename():empty())

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
