--!strict
local Graphics = luWaw.GetModule('Graphics')
local FileSystem = luWaw.GetModule('FileSystem')
local Window = luWaw.GetModule('Window')
Window.Maximize()
Graphics.SetBlendMode('blend')

local RESOURCE_DIR = Path("resources")
local fps_label = {
    accumulated = 0,
    frame_count = 0,
    label = "fps",
}
print(`Exe = {FileSystem.ExecutablePath()}`)
for i, entry in FileSystem.GetChildrenOf(FileSystem.ExecutablePath()) do
    print(`   entry: {entry:Path()}`)
end
fps_label.update_connection = luWaw.OnUpdate:Connect(function(dt: number)
    local self = fps_label
    self.frame_count += 1
    self.accumulated += dt
    if (self.accumulated >= 1) then
        self.label = `{self.frame_count} fps`
        self.frame_count = 0
        self.accumulated = 0
    end
end)

local a = Matrix3.fromTranslation(Vector2(100, 100))
local g = Matrix3(
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1}
)
local a_rect = Rectangle(100, 100, 100, 100)
print(a_rect, a_rect.Width, a_rect.Height, a_rect.X)
print(g, g * a)
print(Matrix3.fromScale(Vector2(10, 20)))

local jw = Texture(RESOURCE_DIR / "jw.png")
jw.BlendMode = "blend"
jw.Color = Color(255, 255, 255)

local my_event: Event<number> = Event()

my_event:Connect(function(num: number)
    Window.SetTitle(`yo check this number out {num}`)
end)

local jw_angle = 0
local abs = math.abs
local sin = math.sin
local cos = math.cos
local my_font = Font(RESOURCE_DIR / "main_font.ttf", 100)
local hello_text = {'h','e','l','l','o', ' ', 'w', 'o', 'r', 'l','d'}
Graphics.SetBlendMode('blend')
local text_change_accumulated = 0
luWaw.OnUpdate:Connect(function(dt: number)
    text_change_accumulated += dt
    if text_change_accumulated < .02 then
        return
    end
    local c1 = math.random(1, #hello_text)
    local c2 = math.random(1, #hello_text)
    local temp = hello_text[c1]
    hello_text[c1] = hello_text[c2]
    hello_text[c2] = temp
    text_change_accumulated = 0
end)

local col
luWaw.OnRender:Connect(function()
    Graphics.ClearCanvas(Color(0, 0, 0, 255))
    col = Color(
        abs(cos(jw_angle)) * 255,
        abs(sin(jw_angle - 45)) * 255,
        abs(sin(jw_angle)) * 255, 255)
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
    local transformation = Matrix3.fromTranslation(Vector2(50, 400))
    transformation *= Matrix3.fromScale(Vector2(1, cos(jw_angle * 4) * .1 + 1))
    Graphics.DrawString(my_font, table.concat(hello_text), transformation)
end)
luWaw.OnRender:Connect(function()
    local angle = jw_angle
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
luWaw.OnUpdate:Connect(function(dt: number)
    jw_angle += math.rad(30) * dt
end)

local accumulated = 0
local accumulated2 = 0
luWaw.OnUpdate:Connect(function(deltasec: number)
    accumulated += deltasec
    if (accumulated > 10) then
        print(`accumulated: {accumulated} {deltasec}`)
        accumulated = 0;
    end
end)

local connection  
connection = luWaw.OnUpdate:Connect(function(deltasec: number)
    accumulated2 += deltasec
    if (accumulated2 > 5) then
        print(`accumulated2: {accumulated2} {deltasec}`)
        accumulated2 = 0;
        luWaw.OnUpdate:Disconnect(connection);
        my_event:Fire(11000)
    end
end)
fps_label.render_connection = luWaw.OnRender:Connect(function()
    local self = fps_label
    Graphics.SetDrawColor(Color(255, 255, 255))
    Graphics.DrawString(self.label, Matrix3.fromTranslation(Vector2(0, 0)))
end)

luWaw.OnKeyUp:Connect(function(key: KeyCode)
    if key == '0' then
    end
end)

luWaw.OnKeyDown:Connect(function(key: KeyCode)
    if key == 'a' then
        print('up')
    end
end)
