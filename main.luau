--!strict
local std = require("std")
local run = std.run
local event: std.Event<number, string> = std.Event.init()

event:connect(function(num: number, text: string)
    print(`yoo {num}, {text}`)
end)

event:connect(function(a0: number, a1: string)
    print("yee")
end)

local dummy = Physical()
dummy.welded = false

run.on_render(function()
    renderer.fill(dummy:bounds())
end)

run.on_quit(function()
    event:deinit()
end)
