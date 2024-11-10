--!strict
local rendering = engine.module("rendering")
local drawing = engine.module('drawing')
local iter = engine.module("iterator")
local white = Color(0xff, 0xff, 0xff, 0xff)

task.spawn(function()
    print(task.wait(10))
end)

local function on_render()
    drawing.set_color(Color(0xff, 0xff, 0xff, 0xff))
    drawing.fill_rectangle(Rectangle(100, 100, 200, 100))
    rendering.clear(white)
end

local function on_key_up(key: keycode)
    if key == '0' then
    end
end

local function on_key_down(key: keycode)
    if key == 'a' then
        print('up')
    end
end

callbacks.render = on_render
callbacks.keydown = on_key_down
callbacks.keyup = on_key_up
