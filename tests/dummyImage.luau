local Graphics = game.GetModule('Graphics')
local dummy = Texture('resources/jw.png')
dummy.BlendMode = "Blend"
dummy.Color = Color(255, 255, 255)
local angle = 0

local Module = {}
function Module.getDummy()
    return dummy
end

function Module.init()
    game.Rendering:Connect(function()
        local t = Matrix3(
            1, 0, 600 + math.cos(angle /2) * 100,
            0, 1, 200 + math.cos(angle / 3) * 50,
            0, 0, 1
        )
        local r = Matrix3(
            math.cos(angle / 3), -math.sin(angle / 3), 0,
            math.sin(angle * 2), math.cos(angle * 5), 0,
            0, 0, 1
        )
        local s = Matrix3(
            math.sin(angle) * .3, 0, 0,
            0, .3, 0,
            0, 0, 1
        )
        local off = Matrix3(
            1, 0, -dummy.Width / 2,
            0, 1, -dummy.Height / 2,
            0, 0, 1
        )
        local transform = t * r * s * off
        Graphics.DrawTexture(dummy, transform)
    end)
    game.Updating:Connect(function(dt: number)
        angle += math.rad(30) * dt
    end)
end


return Module
