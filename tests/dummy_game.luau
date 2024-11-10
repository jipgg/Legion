--!strict
local rendering = engine.get_module("rendering")
local drawing = engine.get_module('drawing')
local white = Color(0xff, 0xff, 0xff, 0xff)

local my_event: Event<number> = Event()

my_event:connect(function(num: number)
    engine.window.set_title("yoooooooooooooooooooooo")
end)

local accumulated = 0
local accumulated2 = 0
engine.updating:connect(function(deltasec: number)
    accumulated += deltasec
    if (accumulated > 10) then
        print(`accumulated: {accumulated} {deltasec}`)
        accumulated = 0;
    end
end)
engine.rendering:connect(function()
    drawing.set_color(Color(100, 0, 0, 0))
    drawing.fill_rectangle(Rectangle(100, 100, 200, 100))
end)

local connection: number
connection = engine.updating:connect(function(deltasec: number)
    accumulated2 += deltasec
    if (accumulated2 > 5) then
        print(`accumulated2: {accumulated2} {deltasec}`)
        accumulated2 = 0;
        engine.updating:disconnect(connection);
        my_event:fire(11000)
    end
end)

engine.key_released:connect(function(key: keycode)
    if key == '0' then
    end
end)

engine.key_pressed:connect(function(key: keycode)
    if key == 'a' then
        print('up')
    end
end)
