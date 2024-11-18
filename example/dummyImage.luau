local Graphics = game.GetModule('Graphics')

local M = {
    dummy = Texture('resourves/jw.png'),
    angle = 0,
}
export type Module = {
    init: ()->(),
    dummy: Texture,
}
function M.init()
    M.dummy.BlendMode = 'Blend'
    M.dummy.Color = Color(255, 255, 255)
    game.Rendering:Connect(function()
        local t = Matrix3(
            1, 0, 600 + math.cos(M.angle /2) * 100,
            0, 1, 200 + math.cos(M.angle / 3) * 50,
            0, 0, 1
        )
        local r = Matrix3(
            math.cos(M.angle / 3), -math.sin(M.angle / 3), 0,
            math.sin(M.angle * 2), math.cos(M.angle * 5), 0,
            0, 0, 1
        )
        local s = Matrix3(
            math.sin(M.angle) * .3, 0, 0,
            0, .3, 0,
            0, 0, 1
        )
        local off = Matrix3(
            1, 0, -M.dummy.Width / 2,
            0, 1, -M.dummy.Height / 2,
            0, 0, 1
        )
        local transform = t * r * s * off
        Graphics.DrawTexture(M.dummy, transform)
    end)
    game.Updating:Connect(function(dt: number)
        M.angle += math.rad(30) * dt
    end)
end

return M :: Module
