local Graphics = core.get_module('Graphics')
local grender = Graphics.render

local M = {
    dummy = Texture('resources/jw.png'),
    angle = 0,
}
export type Module = {
    init: ()->(),
    dummy: Texture,
}
function M.init()
    M.dummy.blend_mode = 'Blend'
    M.dummy.color = Color(255, 255, 255)
    core.during_render:connect(function()
        local t = Mat3(
            1, 0, 600 + math.cos(M.angle /2) * 100,
            0, 1, 200 + math.cos(M.angle / 3) * 50,
            0, 0, 1
        )
        local r = Mat3(
            math.cos(M.angle / 3), -math.sin(M.angle / 3), 0,
            math.sin(M.angle * 2), math.cos(M.angle * 5), 0,
            0, 0, 1
        )
        local s = Mat3(
            math.sin(M.angle) * .3, 0, 0,
            0, .3, 0,
            0, 0, 1
        )
        local off = Mat3(
            1, 0, -M.dummy.width / 2,
            0, 1, -M.dummy.height / 2,
            0, 0, 1
        )
        local transform = t * r * s * off
        grender.texture(M.dummy, transform)
    end)
    core.during_update:connect(function(dt: number)
        M.angle += math.rad(30) * dt
    end)
end

return M :: Module
