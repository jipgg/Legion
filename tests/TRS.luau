type Scale = ((s: number) -> M3x3) & ((sx: number, sy: number) -> M3x3)
export type TRS = {
    translation: (x: number, y: number) -> M3x3,
    scale: Scale,
    rotation: (rad: number) -> M3x3,
}
local Module = {}
function Module.translation(x: number, y: number): M3x3
    return M3x3(
        1, 0, x,
        0, 1, y,
        0, 0, 1
    )
end

local scale: Scale = function(sx, sy)
    sy = if sy then sy else sx
    --sy = sy or sx
    return M3x3(
        sx, 0, 0,
        0, sy, 0,
        0, 0, 1
    )
end
Module.scale = scale
function Module.rotation(rad: number): M3x3
    return M3x3(
        math.cos(rad), -math.sin(rad), 0,
        math.sin(rad), math.cos(rad), 0,
        0, 0, 1
    )
end
return Module::TRS
