--!strict
local rendering = engine.get_module("rendering")
local drawing = engine.get_module('drawing')
local white = Color(0xff, 0xff, 0xff, 0xff)

local b = fixed_buffer.create('double', 40)
fixed_buffer.write('double', b, 1, 100)
print(fixed_buffer.read('double', b, 1))
print(fixed_buffer.len('double', b), buffer.len(b::any))

local my_event: Event<number> = Event()

my_event:connect(function(num: number)
    engine.window.set_title("yoooooooooooooooooooooo")
end)

type polygon = {Vector2}
local plgn: polygon = {
    Vector2(100, 120),
    Vector2(200, 100),
    Vector2(200, 200),
    Vector2(100, 230),
}
engine.rendering:connect(function()
    rendering.clear(Color())
    drawing.set_color(Color(100, 0, 0, 0))
    drawing.fill_rectangle(Rectangle(100, 100, 200, 100))
    --drawing.fill_circle(Vector2(500, 400), 50)
    drawing.fill_ellipse(Vector2(500, 400), Vector2(100, 10))
    --drawing.fill_polygon(plgn)
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

local connection: event_connection_id
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
