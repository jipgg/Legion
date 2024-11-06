--!strict
local sdl = cimport 'SDL'
local Buffer = require 'Buffer'
local trs = require 'TRS'
local Image = {}
Image.__index = Image
type Fields = {
    anchorPoint: Vec2?,
    texturePath: Path,
}
local function construct(t: Fields)
    local texture, width, height = assert(sdl.loadImage(t.texturePath))
    local this = setmetatable({
        texture = texture,
        width = width,
        height = height,
        anchorPoint = t.anchorPoint or Vec2(0, 0),
        renderBuffer = Buffer(90),
    }, Image)
    return this
end

function Image:_applyTransform(transform: Mat3x3, x: number, y: number): Vec3
    self = self::Image
    local ap = self.anchorPoint
    local v = Vec3(x, y, 1)
    local offset = trs.T(-ap.x * self.width, -ap.y * self.height)
    return transform * offset * v
end

function Image:render(trs: Mat3x3?)
    self = self::Image
    local buf = self.renderBuffer
    local width = self.width
    local height = self.height
    buf.size = 0
    local xyType: 'f32' = 'f32'
    local xyOffset = buf.size
    if trs then
        local v0 = self:_applyTransform(trs, 0, 0)
        local v1 = self:_applyTransform(trs, width, 0)
        local v2 = self:_applyTransform(trs, width, height)
        local v3 = self:_applyTransform(trs, 0, height)
        buf:push(xyType,
            v0.x, v0.y,
            v1.x, v1.y,
            v2.x, v2.y,
            v3.x, v3.y
        )
    else
        buf:push(xyType,
            0, 0,
            width, 0,
            width, height,
            0, height
        )
    end
    local colorOffset = buf.size
    buf:push('u8', 0xff, 0xff, 0xff, 0xff)
    local uvType: 'f32' = 'f32'
    local uvOffset = buf.size 
    buf:push(uvType,
        0, 0,
        1, 0,
        1, 1,
        0, 1
    )
    local indicesType: 'i8' = 'i8'
    local indicesOffset = buf.size
    buf:push(indicesType,
        0, 1, 2,
        2, 3, 0
    )
    local numVertices = 4
    sdl.render.renderGeometryRaw(
        (self::Image).texture, buf.data,
        xyOffset, 2 * buf:sizeof(xyType),
        colorOffset, 0,
        uvOffset, 2 * buf:sizeof(uvType),
        numVertices,
        indicesOffset, 6, buf:sizeof(indicesType)
    )
end

export type Image = typeof(construct({}::Fields))

return construct
