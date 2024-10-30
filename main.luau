--!strict
local fs = require '@Legion/filesystem'
local state = require '@Legion/state'
local resources = Path("resources");
local jw = resources / "jw.png"
print(typeof(jw))
local font = sdl.open_font(tostring(resources / "main_font.ttf"), 16);
if font then
    print("nice font bro")
end
local p = V2(100, 100)
print(p)
local pi = V2i(p)
print(pi)
if fs.exists(jw) then
    local texture, w, h = sdl.load_image(tostring(jw))
    if texture then
        print(typeof(texture), w, h)
        print("ok")
        state.rendering:connect(function()
            sdl.render.set_draw_color(Coloru32(0xffffffff))
            sdl.render.render_copy(texture, Recti(0, 0, w * .5, h * .5))
        end)
    else
        print("not ok")
    end
end
