--!strict
local gr = engine.get_module('graphics')
local res_dir = fs.path("resources")
local fps_label = {
    accumulated = 0,
    frame_count = 0,
    label = "fps",
}
fps_label.update_connection = engine.updating:connect(function(dt: number)
    local self = fps_label
    self.frame_count += 1
    self.accumulated += dt
    if (self.accumulated >= 1) then
        self.label = `{self.frame_count} fps`
        self.frame_count = 0
        self.accumulated = 0
    end
end)

local a = matrix3.from_position(vector2(100, 100))
matrix3()
local g = matrix3(
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1}
)
local a_rect = rectangle(100, 100, 100, 100)
print(a_rect, a_rect.width, a_rect.height, a_rect.x)
print(g, g * a)
print(matrix3.from_scale(vector2(10, 20)))

--local jw = engine.load_image(res_dir / "jw.png")
local jw = texture(res_dir / "jw.png")
jw.blend_mode = "blend"
jw.color = color(255, 255, 255, 255)

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
local hello_text = {'h','e','l','l','o', ' ', 'w', 'o', 'r', 'l','d'}
gr.set_blend_mode('blend')
local text_change_accumulated = 0
engine.updating:connect(function(dt: number)
    text_change_accumulated += dt
    if text_change_accumulated < .02 then
        return
    end
    local c1 = math.random(1, #hello_text)
    local c2 = math.random(1, #hello_text)
    local temp = hello_text[c1]
    hello_text[c1] = hello_text[c2]
    hello_text[c2] = temp
    text_change_accumulated = 0
end)

local col
engine.rendering:connect(function()
    gr.clear_canvas(color(0, 0, 0, 255))
    col = color(
        abs(cos(jw_angle)) * 255,
        abs(sin(jw_angle - 45)) * 255,
        abs(sin(jw_angle)) * 255, 255)
    --local c_inverted = color(0xff - c.red, 0xff - c.green, 0xff - c.blue, 0xff)
    gr.set_color(col:alpha_blend(col))
    gr.fill_circle(vector2(100, 100), 40)
    gr.set_color(col:multiply(col:modulate(col)))
    gr.fill_rectangle(rectangle(200-40, 100-40, 80, 80))
    gr.set_color(col:multiply(col:invert()))
    gr.fill_rectangle(rectangle(200-40, 200-40, 80, 80))
    gr.set_color(col:modulate(col))
    gr.fill_circle(vector2(100, 200), 40)
    gr.set_color(color(col.red, col.green, col.blue, 100):invert())
    gr.set_blend_mode('blend')
    gr.fill_circle(vector2(500, 400), 450)
    gr.set_color(color(col.red, col.green, col.blue, 50))
    gr.fill_ellipse(vector2(400, 480), vector2(400, 100));
    gr.set_color(color(col.red, col.green, col.blue, 50):invert():modulate(col))
    gr.fill_ellipse(vector2(400, 300), vector2(200, 400));
    gr.set_color(color(col.red, col.green, col.blue, 50):invert():multiply(col))
    gr.fill_ellipse(vector2(400, 480), vector2(400, 300));
    gr.set_color(color(100, 100, 100):additive_blend(col))
    local transformation = matrix3.from_position(vector2(50, 400))
    transformation *= matrix3.from_scale(vector2(1, cos(jw_angle * 4) * .1 + 1))
    gr.draw_string(my_font, table.concat(hello_text), transformation)
end)
engine.rendering:connect(function()
    local angle = jw_angle
    local t = matrix3(
        1, 0, 600 + math.cos(angle /2) * 100,
        0, 1, 200 + math.cos(angle / 3) * 50,
        0, 0, 1)
    local r = matrix3(
        math.cos(angle / 3), -math.sin(angle / 3), 0,
        math.sin(angle * 2), math.cos(angle * 5), 0,
        0, 0, 1)
    local s = matrix3(
        math.sin(angle) * .3, 0, 0,
        0, .3, 0,
        0, 0, 1)
    local off = matrix3(
        1, 0, -jw.width / 2,
        0, 1, -jw.height / 2,
        0, 0, 1
    )
    local transform = t * r * s * off
    --gr.set_color(color(200, 200, 200):additive_blend(col))
    gr.draw_texture(jw, transform)
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
fps_label.render_connection = engine.rendering:connect(function()
    local self = fps_label
    gr.set_color(color(255, 255, 255, 255))
    gr.draw_string(self.label, matrix3.from_position(vector2(0, 0)))
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
