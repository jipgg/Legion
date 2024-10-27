--!strict
local lg = require("@legion")
local state = lg.state

local dummy = Physical()
dummy.velocity = Vec2(100, 100)
local a = dummy:bounds();
print(dummy.velocity:unit())
print(dummy.size.x)
print(a)
print(builtin.title, builtin.window_size)
local rect = Rect(100, 100, 200, 100)
local my_button = lg.Button.new{
    hit_area = Rect(200, 200, 100, 100)
}
my_button.clicked:connect(function(pos: Vec2)
    print("clicked")
    local w = my_button.hit_area.width
    local h = my_button.hit_area.height
    my_button.hit_area = Rect(math.random(0, builtin.window_size.x - w),
        math.random(0, builtin.window_size.y - h), w, h)
end)


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
