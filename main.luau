--!strict
local lg = require("legion_luau")
local game = lg.game
local mouse = game.mouse

local dummy = Physical()
dummy.velocity = Vec2(100, 100)
local a = dummy:bounds();
print(dummy.velocity:unit())
print(dummy.size.x)
print(a)
print(builtin.title, builtin.window_size)

mouse.button_down:connect(function(button: lg.Mouse_button, pos: Vec2)
    print(builtin.get_mouse_position(), pos)
    print(`mouse button down {button} {pos}`)
end)
mouse.button_up:connect(function(button: lg.Mouse_button, pos: Vec2)
    print(`mouse button up {button} {pos}`)
end)
game.key_down:connect(function(key: Key)
    print(key)
    if key == 'W' then
        print('yoooo')
    end
end)
