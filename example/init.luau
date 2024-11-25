--!strict
local Graphics = waw.get_builtin_module('Graphics')
local fpsCounter = require('@example/fpsCounter')
local dummyImage = require('@example/dummyImage')
local Files = waw.get_builtin_module('Files')
local V2 = Vec2
local M3 = Mat3
local Rct = Rect
local Col = Color
local RESOURCE_DIR = Files.Path("resources")
print(`is clip enabled {Graphics.is_clip_enabled()}`)
print(`is clip enabled {Graphics.is_clip_enabled()}`)
Graphics.blendMode = 'Blend'
waw.window.set_icon(tostring(RESOURCE_DIR :: Files_Path / "coin.png"))
waw.window.title = "Yoooooo"
waw.window.position = V2(100, 100)
--core.window.maximize()
local re = Recti(0, 0, 100, 100);
print(re.x, re.y, re.width, re.height)
print(re, re.x)
print(waw.window.size)
--Window.Borderless = true

local myEvent = Event()

myEvent:connect(function(num: number)
    waw.window.title = `Yo check this number out {num}`
    waw.window.opacity = 0.5
    waw.window.borderless = false
end)
local currentAngle = 0
local myFont = Font(RESOURCE_DIR / "main_font.ttf", 100)
local helloText = {'h','e','l','l','o', ' ', 'w', 'o', 'r', 'l','d'}
local textChangeAccumulated = 0
waw.onUpdate:connect(function(dt: number)
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

waw.onRender:connect(function()
    local color
    Graphics.clear(Color(0, 0, 0, 255))
    color = Color(
        math.abs(math.cos(currentAngle)) * 255,
        math.abs(math.sin(currentAngle - 45)) * 255,
        math.abs(math.sin(currentAngle)) * 255
    )
    --Graphics.ClipRect = Rectangle(0, 0, Window.Size.X / 2, Window.Size.Y / 2)
    local SIZE = waw.window.size
    Graphics.viewport = Rct(0, 0, SIZE.x / 2, SIZE.y)
    Graphics.color = color:alpha_blend(color)
    Graphics.fill.circle(V2(100, 100), 40)
    Graphics.color = color:multiplied(color:modulated(color))
    Graphics.fill.rect(Rct(200-40, 100-40, 80, 80))
    Graphics.color = color:multiplied(color:inverted())
    
    Graphics.fill.rect(Rct(200-40, 200-40, 80, 80))
    Graphics.color = color:modulated(color)
    Graphics.fill.circle(V2(100, 200), 40)
    Graphics.color = Col(color.red, color.green, color.blue, 100):inverted()
    Graphics.blendMode = 'Blend'
    Graphics.fill.circle(V2(500, 400), 450)
    Graphics.color = Col(color.red, color.green, color.blue, 50)
    Graphics.fill.ellipse(V2(400, 480), V2(400, 100));
    Graphics.color = Col(color.red, color.green, color.blue, 50):inverted():modulated(color)
    Graphics.fill.ellipse(V2(400, 300), V2(200, 400));
    Graphics.color = Col(color.red, color.green, color.blue, 50):inverted():multiplied(color)
    Graphics.fill.ellipse(V2(400, 480), V2(400, 300));
    Graphics.color = Col(100, 100, 100):additive_blend(color)
    local transformation = M3.from_translation(V2(50, 400))
    transformation *= M3.from_scale(V2(1, math.cos(currentAngle * 4) * .1 + 1))

    local S = Rct(SIZE.x / 2, 0, SIZE.x / 2, SIZE.y / 2)
    Graphics.viewport = S
    Graphics.blendMode = "None"
    Graphics.color = Col(255, 255, 255)
    Graphics.fill.rect(Rct(0, 0, SIZE.x / 2, SIZE.y / 2))
    Graphics.blendMode = "Blend"
    Graphics.color = Col(100, 100, 100):additive_blend(color)
    Graphics.draw.string(myFont, table.concat(helloText), transformation)
    Graphics.viewport = Rct(0, 0, SIZE.x, SIZE.y)
end)
waw.onUpdate:connect(function(dt: number)
    currentAngle += math.rad(30) * dt
end)
fpsCounter.init()
dummyImage.init()
waw.userinput.onMouseDown:connect(function(type: "Left" | "Middle" | "Right", mouse_pos: Vec2)
end)
waw.userinput.onKeyDown:connect(function(key)
    if key == 'W' then
        print(`Mouse position: {waw.userinput.mousePosition}`)
    elseif key == 'A' then
        Graphics.vsyncOn = not Graphics.vsyncOn
    elseif key == 'D' then
        Graphics.clipRect = nil
        print(`is clip enabled {Graphics.is_clip_enabled()}`)
    end
end)
