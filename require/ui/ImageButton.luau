local Image = require '../Image'
local run = require '../run'
local trs = require '../TRS'
local input = require '../input'
local Event = require '../Event'

local ImageButton = {}
ImageButton.__index = ImageButton
function ImageButton:_onRender()
    self = self::ImageButton
    local pos = self.position
    local rot = self.rotation
    local scale = self.scale
    local transform = trs.T(pos.x, pos.y) * trs.R(rot) * trs.S(scale.x, scale.y)
    self.image:render(transform)
end

function ImageButton.new(texturePath: Path, anchorPoint: Vec2?, position: Vec2?, scale: Vec2?, rotation: number?)
    local self = setmetatable({
        image = Image.new(texturePath, anchorPoint),
        position = position or Vec2(),
        scale = scale or Vec2(1, 1),
        rotation = rotation or 0,
        clicked = Event.new():signal_cast()::Event.Signal<MouseButtonType, Vec2>,
    }, ImageButton)
    self._renderConnection = run.onRender:connect(function() (self::any):_onRender() end)
    self._clickedConnection = input.onMouseReleased:connect(function(button: MouseButtonType, pos: Vec2)
        -- import collision function from cpp
        Event.reveal(self.clicked):fire(button, pos)
    end)
    return self
end

function ImageButton:__dtor()
    self = self::ImageButton
    if self._renderConnection then
        self._renderConnection:disconnect()
    end
    if self._clickedConnection then
        self._clickedConnection:disconnect()
    end
end

export type ImageButton = typeof(ImageButton.new(Path("")))

return ImageButton
