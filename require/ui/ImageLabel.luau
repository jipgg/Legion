local Image = require '../Image'
local run = require '../run'
local trs = require '../TRS'

local ImageLabel = {}
ImageLabel.__index = ImageLabel
function ImageLabel:_onRender()
    self = self::ImageLabel
    local pos = self.position
    local rot = self.rotation
    local scale = self.scale
    local transform = trs.T(pos.x, pos.y) * trs.R(rot) * trs.S(scale.x, scale.y)
    self.image:render(transform)
end

function ImageLabel.new(texturePath: Path, anchorPoint: Vec2?, position: Vec2?, scale: Vec2?, rotation: number?)
    local self = setmetatable({
        image = Image.new(texturePath, anchorPoint),
        position = position or Vec2(),
        scale = scale or Vec2(1, 1),
        rotation = rotation or 0,
    }, ImageLabel)
    self._renderConnection = run.onRender:connect(function() (self::any):_onRender() end)
    return self
end

function ImageLabel:destroy()
    self = self::ImageLabel
    self._renderConnection:disconnect()
end

export type ImageLabel = typeof(ImageLabel.new(Path("")))

return ImageLabel
