--!strict
local fs = builtin.load_module('filesystem')
print(fs.absolute(fs.Path('resources')))
local rendering = builtin.load_module("rendering")
local window = builtin.load_module('window')
local drawing = builtin.load_module('drawing')
window.on_size_change = function(width, height)
    print(`resize called: {width} {height}`)
end



window.on_move = function(x, y)
    print("aaaaa", x, y)
end

local a = {}
local a = rawlen(a)


local white = Color(0xff, 0xff, 0xff, 0xff)
builtin.on_render = function()
    drawing.set_color(Color(0xff, 0xff, 0xff, 0xff))
    drawing.fill_rectangle(Rectangle(100, 100, 200, 100))
    rendering.clear(white)
end

builtin.on_mouseup = function()
end
builtin.on_mousedown = function()
end

builtin.on_keyup = function(key)
    if key == 'a' then
        print('up')
    end
end

builtin.on_keydown = function(key)
    if key == 'a' then
        print('down')
    end
end

