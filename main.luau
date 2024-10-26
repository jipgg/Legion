--!strict
local lg = require("legion_luau")
local state = lg.state

local dummy = Physical()
dummy.velocity = Vec2(100, 100)
local a = dummy:bounds();
print(dummy.velocity:unit())
print(dummy.size.x)
print(a)
print(builtin.title, builtin.window_size)

state.updating:connect(function(delta: number)
    local is_down = builtin.key_down("w")
    if is_down then
        print(`w is down`)
    end
end)

state.key_pressed:connect(function(key: Key)
    if key == "a" then
        print("down")
    end
end)
state.key_released:connect(function(key: Key)
    if key == "a" then
        print("up")
    end
end)
