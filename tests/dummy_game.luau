--!strict
local Graphics = game.GetModule('Graphics')
local Files = game.GetModule('Files')
local Window = game.GetModule('Window')
local RESOURCE_DIR = FilePath("resources")
Graphics.SetBlendMode('blend')
Window.SetIcon(tostring(RESOURCE_DIR / "coin.png"))
Window.Title = "Yoooooo"
Window.Size = Vector2(1240, 720)
Window.Position = Vector2(0, 20)
Window.Borderless = true

print(`Exe = {Files.ExecutablePath()}`)
for i, entry in Files.GetChildrenOf(Files.ExecutablePath()) do
    print(`   entry: {entry:Path()}`)
end

local a = Matrix3.FromTranslation(Vector2(100, 100))
local g = Matrix3(
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1}
)
local aRect = Rectangle(100, 100, 100, 100)
print(aRect, aRect.Width, aRect.Height, aRect.X)
print(g, g * a)
print(Matrix3.FromScale(Vector2(10, 20)))

local jw = Texture(RESOURCE_DIR / "jw.png")
jw.BlendMode = "blend"
jw.Color = Color(255, 255, 255)

local myEvent: Event<number> = Event()

myEvent:Connect(function(num: number)
    Window.Title = `Yo check this number out {num}`
    Window.Opacity = 0.5
    Window.Borderless = false
end)
local fpsLabel = {
    Accumulated = 0,
    FrameCount = 0,
    Label = "fps",
}
fpsLabel.UpdateConnection = game.OnUpdate:Connect(function(dt: number)
    local self = fpsLabel
    self.FrameCount += 1
    self.Accumulated += dt
    if (self.Accumulated >= 1) then
        self.Label = `{self.FrameCount} fps`
        self.FrameCount = 0
        self.Accumulated = 0
    end
end)

local jwAngle = 0
local abs = math.abs
local sin = math.sin
local cos = math.cos
local myFont = Font(RESOURCE_DIR / "main_font.ttf", 100)
local helloText = {'h','e','l','l','o', ' ', 'w', 'o', 'r', 'l','d'}
Graphics.SetBlendMode('blend')
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

local col
game.OnRender:Connect(function()
    Graphics.ClearCanvas(Color(0, 0, 0, 255))
    col = Color(
        abs(cos(jwAngle)) * 255,
        abs(sin(jwAngle - 45)) * 255,
        abs(sin(jwAngle)) * 255, 255)
    --local c_inverted = color(0xff - c.red, 0xff - c.green, 0xff - c.blue, 0xff)
    Graphics.SetDrawColor(col:AlphaBlend(col))
    Graphics.FillCircle(Vector2(100, 100), 40)
    Graphics.SetDrawColor(col:Multiply(col:Modulate(col)))
    Graphics.FillRectangle(Rectangle(200-40, 100-40, 80, 80))
    Graphics.SetDrawColor(col:Multiply(col:Invert()))
    Graphics.FillRectangle(Rectangle(200-40, 200-40, 80, 80))
    Graphics.SetDrawColor(col:Modulate(col))
    Graphics.FillCircle(Vector2(100, 200), 40)
    Graphics.SetDrawColor(Color(col.Red, col.Green, col.Blue, 100):Invert())
    Graphics.SetBlendMode('blend')
    Graphics.FillCircle(Vector2(500, 400), 450)
    Graphics.SetDrawColor(Color(col.Red, col.Green, col.Blue, 50))
    Graphics.FillEllipse(Vector2(400, 480), Vector2(400, 100));
    Graphics.SetDrawColor(Color(col.Red, col.Green, col.Blue, 50):Invert():Modulate(col))
    Graphics.FillEllipse(Vector2(400, 300), Vector2(200, 400));
    Graphics.SetDrawColor(Color(col.Red, col.Green, col.Blue, 50):Invert():Multiply(col))
    Graphics.FillEllipse(Vector2(400, 480), Vector2(400, 300));
    Graphics.SetDrawColor(Color(100, 100, 100):AdditiveBlend(col))
    local transformation = Matrix3.FromTranslation(Vector2(50, 400))
    transformation *= Matrix3.FromScale(Vector2(1, cos(jwAngle * 4) * .1 + 1))
    Graphics.DrawString(myFont, table.concat(helloText), transformation)
end)
game.OnRender:Connect(function()
    local angle = jwAngle
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
        1, 0, -jw.Width / 2,
        0, 1, -jw.Height / 2,
        0, 0, 1
    )
    local transform = t * r * s * off
    --gr.set_color(color(200, 200, 200):additive_blend(col))
    Graphics.DrawTexture(jw, transform)
end)
game.OnUpdate:Connect(function(dt: number)
    jwAngle += math.rad(30) * dt
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
    Graphics.SetDrawColor(Color(255, 255, 255))
    Graphics.DrawString(self.Label, Matrix3.FromTranslation(Vector2(0, 0)))
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
