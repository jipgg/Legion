--!strict
local legion = require '@legion'
local Storage = require 'storage'
local ImageLabel = require '@legion/ui/ImageLabel'
local resourcePath = Path("resources");
local ImageButton = require '@legion/ui/ImageButton'
local imagePath = resourcePath / "coin.png"

local function dereference(obj: any)
    if obj == nil then return end
    if obj.__dtor then obj:__dtor() end
    table.clear(obj::any)
    obj = nil
end

local spinning_angle = 0

local spinning: boolean = true 
legion.input.onMousePressed:connect(function(type: MouseButtonType, mousePosition: Vec2)
    spinning = not spinning
end)
local image = ImageLabel.new(imagePath, Vec2(.5, .5))

Storage += legion.run.onUpdate:connect(function(deltaSeconds: number)
    if spinning then
        local spin_speed = 100
        spinning_angle += spin_speed * deltaSeconds
    end
    image.position = Vec2(300, 300)
    image.scale = Vec2(1 * math.sin(math.rad((spinning_angle * 2) % 180) ), 1)
    image.rotation = math.rad(spinning_angle * 2)
end)
