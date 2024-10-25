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

mouse.button_down:connect(function(button: lg.Mouse_button, pos: Vec2)
    print(`mouse button down {button} {pos}`)
end)
mouse.button_up:connect(function(button: lg.Mouse_button, pos: Vec2)
    print(`mouse button up {button} {pos}`)
end)
