--!strict
local fs = require '@Legion/filesystem'
local state = require '@Legion/state'

local rect = Recti64(100, 100, 200, 100)
local resources = fs.path("resources");
local jw: fs.path = resources / "jw.png"
local font = sdl.open_font(tostring(resources / "main_font.ttf"), 16);
if font then
    print("nice font bro")
end
local p = SDL_Point(100, 100)
print(p)
if fs.exists(jw) then
    local texture, w, h = sdl.load_image(tostring(jw))
    if texture then
        print(typeof(texture), w, h)
        print("ok")
        state.rendering:connect(function()
            sdl.render.set_draw_color(Coloru32(0xffffffff))
            sdl.render.render_copy(texture, SDL_Rect(0, 0, w * .2, h * .2))
        end)
    else
        print("not ok")
    end
end

state.key_pressed:connect(function(key: keycode)
    if key == "a" then
        local mouse_pos = builtin.mouse_pos()
        if builtin.point_in_rect(mouse_pos, rect) then
            print("Yes")
        end
    end
end)
state.key_released:connect(function(key: keycode)
    if key == "a" then
        print("up")
    end
end)
state.mouse_pressed:connect(function(mouse: mouse_button_type, pos: Vec2d)
end)
