local cos = math.cos
local sin = math.sin
local m3x3 = builtin('matrix33')
local trs = {}
function trs.translate(x: number, y: number): matrix33
    return m3x3(
        1, 0, x,
        0, 1, y,
        0, 0, 1
    )
end

type scale_fn = ((s: number) -> matrix33) & ((sx: number, sy: number) -> matrix33)
local scale: scale_fn = function(sx, sy)
    sy = if sy then sy else sx
    --sy = sy or sx
    return m3x3(
        sx, 0, 0,
        0, sy, 0,
        0, 0, 1
    )
end
trs.scale = scale
function trs.rotate(rad: number): matrix33
    return m3x3(
        cos(rad), -sin(rad), 0,
        sin(rad), cos(rad), 0,
        0, 0, 1
    )
end

return trs
