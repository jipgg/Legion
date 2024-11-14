local transform = {}
local cos = math.cos
local sin = math.sin
type scalar = number | vector2

function transform.translate(t: vector2)
    return matrix3(
        1, 0, t.x,
        0, 1, t.y,
        0, 0, 1
    )
end

function transform.rotate(a: number)
    return matrix3(
        cos(a), -sin(a), 0,
        sin(a), cos(a), 0,
        0, 0, 1
    )
end

function transform.scale(s: scalar)
    if typeof(s) == 'vector2' then
        return matrix3(
            s.x, 0, 0,
            0, s.y, 0,
            0, 0, 1
        )
    elseif typeof(s) == 'number' then
        return matrix3(
            s, 0, 0,
            0, s, 0,
            0, 0, 1
        )
    end
    error(`invalid argument type '{typeof(s)}'.`)
end
function transform.all(t: vector2, r: number, s: scalar): matrix3
    return transform.translate(t) * transform.rotate(r) * transform.scale(s)
end

return transform
