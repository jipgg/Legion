--!strict
local render = engine.get_module("rendering")
local grph = engine.get_module('graphics')
local res_dir = fs.path("resources")
local transform = require('@tests/transform')

local a = matrix3.from_position(vector2(100, 100))
local g = matrix3(
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1}
)
print(g, g * a)
print(matrix3.from_scale(vector2(10, 20)))

local jw = engine.load_image(res_dir / "jw.png")

local b = fixed_buffer.create('double', 40)
fixed_buffer.write('double', b, 1, 100)
print(fixed_buffer.read('double', b, 1))
print(fixed_buffer.len('double', b), buffer.len(b::any))
local my_event: event<number> = event()

my_event:connect(function(num: number)
    engine.window.set_title(`yo check this number out {num}`)
end)

local jw_angle = 0
local abs = math.abs
local sin = math.sin
local cos = math.cos
local my_font = font(res_dir / "main_font.ttf", 100)
local hello_text = {
    'h','e','l','l','o', ' ', 'w', 'o', 'r', 'l','d'
}
local text_change_accumulated = 0
engine.updating:connect(function(dt: number)
    text_change_accumulated += dt
    if text_change_accumulated < 1 then
        return
    end
    local c1 = math.random(1, #hello_text)
    local c2 = math.random(1, #hello_text)
    local temp = hello_text[c1]
    hello_text[c1] = hello_text[c2]
    hello_text[c2] = temp
    text_change_accumulated = 0
end)

engine.rendering:connect(function()
    grph.clear_canvas(color(0, 0, 0, 255))
    local c = color(
        abs(cos(jw_angle)) * 255,
        abs(sin(jw_angle - 45)) * 255,
        abs(sin(jw_angle)) * 255, 255)
    local c_inverted = color(0xff - c.r, 0xff - c.g, 0xff - c.b, 0xff)
    grph.set_color(c)
    local transformation = transform.translate(vector2(50, 400))
    transformation *= transform.scale(vector2(1, cos(jw_angle * 4) * .1 + 1))
    grph.draw_text(my_font, table.concat(hello_text), transformation)
    grph.fill_circle(vector2(100, 100), 40)
    grph.set_color(c_inverted)
    grph.fill_rectangle(rectangle(200-40, 100-40, 80, 80))
    grph.set_color(c)
    grph.fill_rectangle(rectangle(200-40, 200-40, 80, 80))
    grph.set_color(c_inverted)
    grph.fill_circle(vector2(100, 200), 40)
end)
engine.rendering:connect(function()
    local t = matrix3(
        1, 0, 600,
        0, 1, 200,
        0, 0, 1)
    local angle = jw_angle
    local r = matrix3(
        math.cos(angle), -math.sin(angle), 0,
        math.sin(angle), math.cos(angle), 0,
        0, 0, 1)
    local s = matrix3(
        math.sin(angle) * .3, 0, 0,
        0, .3, 0,
        0, 0, 1)
    local off = matrix3(
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
