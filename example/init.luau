--!strict
local Graphics = core.GetModule('Graphics')
local GDraw = Graphics.Draw
local GFill = Graphics.Fill
local Window = core.GetModule('Window')
local fpsCounter = require('@example/fpsCounter')
local dummyImage = require('@example/dummyImage')
local RESOURCE_DIR = FilePath("resources")
local UserInput = core.GetModule('UserInput')
print(`is clip enabled {Graphics.IsClipEnabled()}`)
print(`is clip enabled {Graphics.IsClipEnabled()}`)
Graphics.BlendMode = 'Blend'
Window.SetIcon(tostring(RESOURCE_DIR / "coin.png"))
Window.Title = "Yoooooo"
Window.Position = Vector2(100, 100)
Window.Maximize()
print(Window.Size)
--Window.Borderless = true

local myEvent: Event<number> = Event()

myEvent:Connect(function(num: number)
    Window.Title = `Yo check this number out {num}`
    Window.Opacity = 0.5
    Window.Borderless = false
end)
local currentAngle = 0
local myFont = Font(RESOURCE_DIR / "main_font.ttf", 100)
local helloText = {'h','e','l','l','o', ' ', 'w', 'o', 'r', 'l','d'}
local textChangeAccumulated = 0
core.DuringUpdate:Connect(function(dt: number)
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

core.DuringRender:Connect(function()
    local color
    Graphics.ClearCanvas(Color(0, 0, 0, 255))
    color = Color(
        math.abs(math.cos(currentAngle)) * 255,
        math.abs(math.sin(currentAngle - 45)) * 255,
        math.abs(math.sin(currentAngle)) * 255
    )
    --Graphics.ClipRect = Rectangle(0, 0, Window.Size.X / 2, Window.Size.Y / 2)
    local SIZE = Window.Size
    Graphics.Viewport = Rectangle(0, 0, SIZE.X / 2, SIZE.Y)
    Graphics.Color = color:AlphaBlend(color)
    GFill.Circle(Vector2(100, 100), 40)
    Graphics.Color = color:Multiply(color:Modulate(color))
    GFill.Rectangle(Rectangle(200-40, 100-40, 80, 80))
    Graphics.Color = color:Multiply(color:Invert())
    
    GFill.Rectangle(Rectangle(200-40, 200-40, 80, 80))
    Graphics.Color = color:Modulate(color)
    GFill.Circle(Vector2(100, 200), 40)
    Graphics.Color = Color(color.Red, color.Green, color.Blue, 100):Invert()
    Graphics.BlendMode = 'Blend'
    GFill.Circle(Vector2(500, 400), 450)
    Graphics.Color = Color(color.Red, color.Green, color.Blue, 50)
    GFill.Ellipse(Vector2(400, 480), Vector2(400, 100));
    Graphics.Color = Color(color.Red, color.Green, color.Blue, 50):Invert():Modulate(color)
    GFill.Ellipse(Vector2(400, 300), Vector2(200, 400));
    Graphics.Color = Color(color.Red, color.Green, color.Blue, 50):Invert():Multiply(color)
    GFill.Ellipse(Vector2(400, 480), Vector2(400, 300));
    Graphics.Color = Color(100, 100, 100):AdditiveBlend(color)
    local transformation = Matrix3.FromTranslation(Vector2(50, 400))
    transformation *= Matrix3.FromScale(Vector2(1, math.cos(currentAngle * 4) * .1 + 1))

    local S = Rectangle(SIZE.X / 2, 0, SIZE.X / 2, SIZE.Y / 2)
    Graphics.Viewport = S
    Graphics.BlendMode = "None"
    Graphics.Color = Color(255, 255, 255)
    GFill.Rectangle(Rectangle(0, 0, SIZE.X / 2, SIZE.Y / 2))
    Graphics.BlendMode = "Blend"
    Graphics.Color = Color(100, 100, 100):AdditiveBlend(color)
    GDraw.String(myFont, table.concat(helloText), transformation)
    Graphics.Viewport = Rectangle(0, 0, SIZE.X, SIZE.Y)
end)
core.DuringUpdate:Connect(function(dt: number)
    currentAngle += math.rad(30) * dt
end)
fpsCounter.init()
dummyImage.init()
UserInput.KeyPressed:Connect(function(key)
    if key == 'W' then
        print(`Mouse position: {UserInput.MousePosition}`)
    elseif key == 'A' then
        Graphics.VSyncEnabled = not Graphics.VSyncEnabled
    elseif key == 'D' then
        Graphics.ClipRect = nil
        print(`is clip enabled {Graphics.IsClipEnabled()}`)
    end
end)
