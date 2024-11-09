--!strict
local fs = builtin.load_module('filesystem')
local rendering = builtin.load_module("rendering")
local window = builtin.load_module('window')
local drawing = builtin.load_module('drawing')
local white = Color(0xff, 0xff, 0xff, 0xff)
local Path = fs.Path
Vector3(100, 100, 100)
Path("al")
print("yo")
callback.mousedown = function()
end
callback.render = function()
    drawing.set_color(Color(0xff, 0xff, 0xff, 0xff))
    drawing.fill_rectangle(Rectangle(100, 100, 200, 100))
    rendering.clear(white)
end

callback.keydown = function(key: keycode)
end

callback.keyup = function(key)
    if key == 'a' then
        print('up')
    end
end

callback.keydown = function(key)
    if key == 'a' then
        print('down')
    end
end

