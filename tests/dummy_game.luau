--!strict
local Graphics = game.GetModule('Graphics')
local Window = game.GetModule('Window')
local fpsCounter = require('@tests/fpsCounter')
local dummyImage = require('@tests/dummyImage')
local RESOURCE_DIR = FilePath("resources")

Graphics.BlendMode = 'Blend'
Window.SetIcon(tostring(RESOURCE_DIR / "coin.png"))
Window.Title = "Yoooooo"
Window.Position = Vector2(100, 100)
Window.Borderless = true


local myEvent: Event<number> = Event()

myEvent:Connect(function(num: number)
    Window.Title = `Yo check this number out {num}`
    Window.Opacity = 0.5
    Window.Borderless = false
end)
local jwAngle = 0
local abs = math.abs
local sin = math.sin
local cos = math.cos
local myFont = Font(RESOURCE_DIR / "main_font.ttf", 100)
local helloText = {'h','e','l','l','o', ' ', 'w', 'o', 'r', 'l','d'}
local textChangeAccumulated = 0
game.Updating:Connect(function(dt: number)
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

local col
game.Rendering:Connect(function()
    Graphics.ClearCanvas(Color(0, 0, 0, 255))
    col = Color(abs(cos(jwAngle)) * 255, abs(sin(jwAngle - 45)) * 255, abs(sin(jwAngle)) * 255, 255)
    Graphics.DrawColor = col:AlphaBlend(col)
    Graphics.FillCircle(Vector2(100, 100), 40)
    Graphics.DrawColor = col:Multiply(col:Modulate(col))
    Graphics.FillRectangle(Rectangle(200-40, 100-40, 80, 80))
    Graphics.DrawColor = col:Multiply(col:Invert())
    Graphics.FillRectangle(Rectangle(200-40, 200-40, 80, 80))
    Graphics.DrawColor = col:Modulate(col)
    Graphics.FillCircle(Vector2(100, 200), 40)
    Graphics.DrawColor = Color(col.Red, col.Green, col.Blue, 100):Invert()
    Graphics.BlendMode = 'Blend'
    Graphics.FillCircle(Vector2(500, 400), 450)
    Graphics.DrawColor = Color(col.Red, col.Green, col.Blue, 50)
    Graphics.FillEllipse(Vector2(400, 480), Vector2(400, 100));
    Graphics.DrawColor = Color(col.Red, col.Green, col.Blue, 50):Invert():Modulate(col)
    Graphics.FillEllipse(Vector2(400, 300), Vector2(200, 400));
    Graphics.DrawColor = Color(col.Red, col.Green, col.Blue, 50):Invert():Multiply(col)
    Graphics.FillEllipse(Vector2(400, 480), Vector2(400, 300));
    Graphics.DrawColor = Color(100, 100, 100):AdditiveBlend(col)
    local transformation = Matrix3.FromTranslation(Vector2(50, 400))
    transformation *= Matrix3.FromScale(Vector2(1, cos(jwAngle * 4) * .1 + 1))
    Graphics.DrawString(myFont, table.concat(helloText), transformation)
end)
game.Updating:Connect(function(dt: number)
    jwAngle += math.rad(30) * dt
end)
fpsCounter.init()
dummyImage.init()
