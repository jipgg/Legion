--!strict
local render = engine.get_module("rendering")
local draw = engine.get_module('drawing')
local res_dir = Path("resources")

local jw = engine.load_image(res_dir / "jw.png")

local b = fixed_buffer.create('double', 40)
fixed_buffer.write('double', b, 1, 100)
print(fixed_buffer.read('double', b, 1))
print(fixed_buffer.len('double', b), buffer.len(b::any))

local my_event: event<number> = Event()

my_event:connect(function(num: number)
    engine.window.set_title("yoooooooooooooooooooooo")
end)

engine.rendering:connect(function()
    render.clear(Color())
    draw.set_color(Color(255, 255, 255, 255))
    draw.draw_text("Hello World")
end)

-- engine.rendering:connect(function()
--     render.clear(Color())
--     draw.set_blend_mode('blend')
--     draw.set_color(Color(255, 0, 0, 100))
--     draw.fill_rectangle(Vec2(100, 100), Vec2(200, 100))
--     draw.fill_rectangle(Rect(200, 100, 100, 200))
--     draw.fill_circle(Vec2(500, 400), 50, false)
--     draw.set_color(Color(0, 0, 255, 20))
--     draw.fill_circle(Vec2(300, 400), 300, true)
--     draw.fill_ellipse(Vec2(500, 400), Vec2(100, 300), false)
--     --render.texture(jw, Rectangle(0, 0, jw.w / 3, jw.h / 3))
-- end)
local jw_angle = 0
engine.rendering:connect(function()
    local t = Mat3(
        1, 0, 200,
        0, 1, 300,
        0, 0, 1)
    local angle = jw_angle
    local r = Mat3(
        math.cos(angle), -math.sin(angle), 0,
        math.sin(angle), math.cos(angle), 0,
        0, 0, 1)
    local s = Mat3(
        math.sin(angle) * .5, 0, 0,
        0, .5, 0,
        0, 0, 1)
    local off = Mat3(
        1, 0, -jw.w / 2,
        0, 1, -jw.h / 2,
        0, 0, 1
    )
    local transform = t * r * s * off
    render.render_texture(jw, transform)
end)
engine.updating:connect(function(dt: number)
    jw_angle += math.rad(30) * dt
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
