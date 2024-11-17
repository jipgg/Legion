--!strict
local Graphics = game.GetModule('Graphics')
local Files = game.GetModule('Files')
local Window = game.GetModule('Window')
local RESOURCE_DIR = FilePath("resources")
Graphics.BlendMode = 'blend'
Window.Title = "Hello World"
Window.Size = Vector2(1240, 720)
Window.Position = Vector2(100, 100)
Window.SetIcon(tostring(RESOURCE_DIR / "coin.png"))
Window.AlwaysOnTop = true

print(`Exe = {Files.ExecutablePath()}`)
for i, entry: DirectoryEntry in Files.GetChildrenOf(Files.ExecutablePath()) do
    print(`   entry: {entry:Path()}`)
end
local dummyImage = Texture(RESOURCE_DIR / "jw.png")
dummyImage.BlendMode = "blend"
dummyImage.Color = Color(255, 255, 255)

local myEvent: Event<number> = Event()

local fpsLabel = {accumulated = 0, frameCount = 0, label = "fps"}

fpsLabel.updateConnection = game.OnUpdate:Connect(function(dt: number)
    local self = fpsLabel
    self.frameCount += 1
    self.accumulated += dt
    if (self.accumulated >= 1) then
        self.label = `{self.frameCount} fps`
        self.frameCount = 0
        self.accumulated = 0
    end
end)

local myAngle = 0
local abs = math.abs
local sin = math.sin
local cos = math.cos
local myFont = Font(RESOURCE_DIR / "main_font.ttf", 100)
local helloText = {'h','e','l','l','o', ' ', 'w', 'o', 'r', 'l','d'}
local textChangeAccumulated = 0
game.OnUpdate:Connect(function(dt: number)
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

local color
game.OnRender:Connect(function()
    Graphics.ClearCanvas(Color(0, 0, 0, 255))
    color = Color(abs(cos(myAngle)) * 255, abs(sin(myAngle - 45)) * 255, abs(sin(myAngle)) * 255, 255)
    Graphics.DrawColor = color:AlphaBlend(color)
    Graphics.FillCircle(Vector2(100, 100), 40)
    Graphics.DrawColor = color:Multiply(color:Modulate(color))
    Graphics.FillRectangle(Rectangle(200-40, 100-40, 80, 80))
    Graphics.DrawColor = color:Multiply(color:Invert())
    Graphics.FillRectangle(Rectangle(200-40, 200-40, 80, 80))
    Graphics.DrawColor = color:Modulate(color)
    Graphics.FillCircle(Vector2(100, 200), 40)
    Graphics.DrawColor = Color(color.Red, color.Green, color.Blue, 100):Invert()
    Graphics.BlendMode = 'blend'
    Graphics.FillCircle(Vector2(500, 400), 450)
    Graphics.DrawColor = Color(color.Red, color.Green, color.Blue, 50)
    Graphics.FillEllipse(Vector2(400, 480), Vector2(400, 100));
    Graphics.DrawColor = Color(color.Red, color.Green, color.Blue, 50):Invert():Modulate(color)
    Graphics.FillEllipse(Vector2(400, 300), Vector2(200, 400));
    Graphics.DrawColor = Color(color.Red, color.Green, color.Blue, 50):Invert():Multiply(color)
    Graphics.FillEllipse(Vector2(400, 480), Vector2(400, 300));
    Graphics.DrawColor = Color(100, 100, 100):AdditiveBlend(color)
    local transformation = Matrix3.FromTranslation(Vector2(50, 400))
    transformation *= Matrix3.FromScale(Vector2(1, cos(myAngle * 4) * .1 + 1))
    Graphics.DrawString(myFont, table.concat(helloText), transformation)
end)
game.OnRender:Connect(function()
    local angle = myAngle
    local t = Matrix3(
        1, 0, 600 + math.cos(angle /2) * 100,
        0, 1, 200 + math.cos(angle / 3) * 50,
        0, 0, 1)
    local r = Matrix3(
        math.cos(angle / 3), -math.sin(angle / 3), 0,
        math.sin(angle * 2), math.cos(angle * 5), 0,
        0, 0, 1)
    local s = Matrix3(
        math.sin(angle) * .3, 0, 0,
        0, .3, 0,
        0, 0, 1)
    local off = Matrix3(
        1, 0, -dummyImage.Width / 2,
        0, 1, -dummyImage.Height / 2,
        0, 0, 1
    )
    local transform = t * r * s * off
    Graphics.DrawTexture(dummyImage, transform)
end)
game.OnUpdate:Connect(function(dt: number)
    myAngle += math.rad(30) * dt
end)

local accumulated = 0
local accumulated2 = 0
game.OnUpdate:Connect(function(deltasec: number)
    accumulated += deltasec
    if (accumulated > 10) then
        print(`accumulated: {accumulated} {deltasec}`)
        accumulated = 0;
    end
end)

local connection  
connection = game.OnUpdate:Connect(function(deltasec: number)
    accumulated2 += deltasec
    if (accumulated2 > 5) then
        print(`accumulated2: {accumulated2} {deltasec}`)
        accumulated2 = 0;
        game.OnUpdate:Disconnect(connection);
        myEvent:Fire(11000)
    end
end)
fpsLabel.render_connection = game.OnRender:Connect(function()
    local self = fpsLabel
    Graphics.DrawColor = Color(255, 255, 255)
    Graphics.DrawString(self.label, Matrix3.FromTranslation(Vector2(0, 0)))
end)

game.OnKeyUp:Connect(function(key: KeyCode)
    if key == '0' then
    end
end)

game.OnKeyDown:Connect(function(key: KeyCode)
    if key == 'a' then
        print('up')
    end
end)
