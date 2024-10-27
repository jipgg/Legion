local lg = require '@Legion'
local state = lg.state

local dummy = Physical()
dummy.velocity = Vec2(100, 100)
local a = dummy:bounds();
print(dummy.velocity:unit())
print(dummy.size.x)
print(a)
print(builtin.title, builtin.window_size)
local rect = Rect(100, 100, 200, 100)
local dir_path = "test"
fs.create_directory(dir_path)
local script_name = "main.luau"
print("we copying")
if fs.copy_file(script_name, dir_path.."/"..script_name) then
    print("copied")
end
print(fs.absolute("main.luau"))
state.key_pressed:connect(function(key: Keycode)
    if key == "a" then
        local mouse_pos = builtin.mouse_pos()
        if builtin.point_in_rect(mouse_pos, rect) then
            print("Yes")
        end
    end
end)
state.key_released:connect(function(key: Keycode)
    if key == "a" then
        print("up")
    end
end)
state.mouse_pressed:connect(function(mouse: Mousebutton, pos: Vec2)
end)
