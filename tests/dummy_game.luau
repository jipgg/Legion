--!strict
local rendering = engine.get_module("rendering")
local drawing = engine.get_module('drawing')
local white = Color(0xff, 0xff, 0xff, 0xff)

local accumulated = 0
local accumulated2 = 0
event_update:connect(function(deltasec: number)
    accumulated += deltasec
    if (accumulated > 10) then
        print(`accumulated: {accumulated} {deltasec}`)
        accumulated = 0;
    end
end)

event_update:connect(function(deltasec: number)
    accumulated2 += deltasec
    if (accumulated2 > 5) then
        print(`accumulated2: {accumulated2} {deltasec}`)
        accumulated2 = 0;
    end
end)

function engine.on_render()
    drawing.fill_rectangle(Rectangle(100, 100, 200, 100))
    rendering.clear(white)
end

function engine.on_keyup(key: keycode)
    if key == '0' then
    end
end

function engine.on_keydown(key: keycode)
    if key == 'a' then
        print('up')
    end
end
