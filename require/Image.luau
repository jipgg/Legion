--!strict
local sdl = builtin('sdl')
local fs = builtin('filesystem')
local vec2 = builtin('vector2')
local vec3 = builtin('vector3')
local buffer_wrapper = require('buffer_wrapper')
local trs = require('TRS')
export type public = {
    texture: opaque_texture,
    width: number,
    height: number,
    anchor_point: vector2,
    render: (public, transform: matrix33?) -> ()
}
local image = {}
image.__index = image
function image_ctor(texture_path: path, anchor_point: vector2?)
    local texture, width, height = assert(sdl.load_image(texture_path))
    local this = setmetatable({
        texture = texture,
        width = width,
        height = height,
        anchor_point = anchor_point or vec2(0, 0),
        render_buffer = buffer_wrapper(90),
    }, image)
    return this
end

function image:apply_transform(transform: matrix33, x: number, y: number): vector3
    self = self::self
    local ap = self.anchor_point
    local v = vec3(x, y, 1)
    local offset = trs.translate(-ap.x * self.width, -ap.y * self.height)
    return transform * offset * v
end

function image:render(trs: matrix33?)
    self = self::self
    local buf = self.render_buffer
    local width = self.width
    local height = self.height
    buf.size = 0
    local xyType: 'f32' = 'f32'
    local xyOffset = buf.size
    if trs then
        local v0 = self:apply_transform(trs, 0, 0)
        local v1 = self:apply_transform(trs, width, 0)
        local v2 = self:apply_transform(trs, width, height)
        local v3 = self:apply_transform(trs, 0, height)
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
    sdl.render_geometry_raw(
        (self::self).texture, buf.data,
        xyOffset, 2 * buf:sizeof(xyType),
        colorOffset, 0,
        uvOffset, 2 * buf:sizeof(uvType),
        numVertices,
        indicesOffset, 6, buf:sizeof(indicesType)
    )
end

export type self = typeof(image_ctor(fs.path(""), nil))

return ((image_ctor) :: unknown) :: (path, vector2) -> public
