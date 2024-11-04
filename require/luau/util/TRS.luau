local cos = math.cos
local sin = math.sin
local Module = {}
function Module.translation(x: number, y: number): Mat3x3
    return Mat3x3(
        1, 0, x,
        0, 1, y,
        0, 0, 1
    )
end

type Scale = ((s: number) -> Mat3x3) & ((sx: number, sy: number) -> Mat3x3)
local scale: Scale = function(sx, sy)
    sy = if sy then sy else sx
    --sy = sy or sx
    return Mat3x3(
        sx, 0, 0,
        0, sy, 0,
        0, 0, 1
    )
end
Module.scale = scale
function Module.rotation(rad: number): Mat3x3
    return Mat3x3(
        cos(rad), -sin(rad), 0,
        sin(rad), cos(rad), 0,
        0, 0, 1
    )
end

function Module.all(x: number, y: number, sx: number, sy: number, rad: number): Mat3x3
    return Mat3x3(
        sx * cos(rad), -sin(rad), x,
        sin(rad), sy * cos(rad), y,
        0, 0, 1
    )
end
return Module
