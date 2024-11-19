--!strict
local Graphics = core.get_module('Graphics')
local GDraw = Graphics.draw
local GFill = Graphics.fill
local Window = core.get_module('Window')
local fpsCounter = require('@example/fpsCounter')
local dummyImage = require('@example/dummyImage')
local RESOURCE_DIR = FilePath("resources")
local UserInput = core.get_module('UserInput')
print(`is clip enabled {Graphics.is_clip_enabled()}`)
print(`is clip enabled {Graphics.is_clip_enabled()}`)
Graphics.blend_mode = 'Blend'
Window.set_icon(tostring(RESOURCE_DIR / "coin.png"))
Window.title = "Yoooooo"
Window.position = Vec2(100, 100)
Window.maximize()
print(Window.size)
--Window.Borderless = true

local myEvent: Event<number> = Event()

myEvent:connect(function(num: number)
    Window.title = `Yo check this number out {num}`
    Window.opacity = 0.5
    Window.borderless = false
end)
local currentAngle = 0
local myFont = Font(RESOURCE_DIR / "main_font.ttf", 100)
local helloText = {'h','e','l','l','o', ' ', 'w', 'o', 'r', 'l','d'}
local textChangeAccumulated = 0
core.during_update:connect(function(dt: number)
    textChangeAccumulated += dt
    if textChangeAccumulated < .02 then
        return
    end
    local c1 = math.random(1, #helloText)
    local c2 = math.random(1, #helloText)
    local temp = helloText[c1]
    helloText[c1] = helloText[c2]
    helloText[c2] = temp
    textChangeAccumulated = 0
end)

core.during_render:connect(function()
    local color
    Graphics.clear(Color(0, 0, 0, 255))
    color = Color(
        math.abs(math.cos(currentAngle)) * 255,
        math.abs(math.sin(currentAngle - 45)) * 255,
        math.abs(math.sin(currentAngle)) * 255
    )
    --Graphics.ClipRect = Rectangle(0, 0, Window.Size.X / 2, Window.Size.Y / 2)
    local SIZE = Window.size
    Graphics.viewport = Rect(0, 0, SIZE.x / 2, SIZE.y)
    Graphics.color = color:alpha_blend(color)
    GFill.circle(Vec2(100, 100), 40)
    Graphics.color = color:multiplied(color:modulated(color))
    GFill.rect(Rect(200-40, 100-40, 80, 80))
    Graphics.color = color:multiplied(color:inverted())
    
    GFill.rect(Rect(200-40, 200-40, 80, 80))
    Graphics.color = color:modulated(color)
    GFill.circle(Vec2(100, 200), 40)
    Graphics.color = Color(color.red, color.green, color.blue, 100):inverted()
    Graphics.blend_mode = 'Blend'
    GFill.circle(Vec2(500, 400), 450)
    Graphics.color = Color(color.red, color.green, color.blue, 50)
    GFill.ellipse(Vec2(400, 480), Vec2(400, 100));
    Graphics.color = Color(color.red, color.green, color.blue, 50):inverted():modulated(color)
    GFill.ellipse(Vec2(400, 300), Vec2(200, 400));
    Graphics.color = Color(color.red, color.green, color.blue, 50):inverted():multiplied(color)
    GFill.ellipse(Vec2(400, 480), Vec2(400, 300));
    Graphics.color = Color(100, 100, 100):additive_blend(color)
    local transformation = Mat3.from_translation(Vec2(50, 400))
    transformation *= Mat3.from_scale(Vec2(1, math.cos(currentAngle * 4) * .1 + 1))

    local S = Rect(SIZE.x / 2, 0, SIZE.x / 2, SIZE.y / 2)
    Graphics.viewport = S
    Graphics.blend_mode = "None"
    Graphics.color = Color(255, 255, 255)
    GFill.rect(Rect(0, 0, SIZE.x / 2, SIZE.y / 2))
    Graphics.blend_mode = "Blend"
    Graphics.color = Color(100, 100, 100):additive_blend(color)
    GDraw.string(myFont, table.concat(helloText), transformation)
    Graphics.viewport = Rect(0, 0, SIZE.x, SIZE.y)
end)
core.during_update:connect(function(dt: number)
    currentAngle += math.rad(30) * dt
end)
fpsCounter.init()
dummyImage.init()
UserInput.key_pressed:connect(function(key)
    if key == 'W' then
        print(`Mouse position: {UserInput.mouse_position}`)
    elseif key == 'A' then
        Graphics.vsync_enabled = not Graphics.vsync_enabled
    elseif key == 'D' then
        Graphics.clip_rect = nil
        print(`is clip enabled {Graphics.is_clip_enabled()}`)
    end
end)
