local Graphics = game.GetModule("Graphics")
local accumulatedSeconds = 0
local frameCount = 0
local label = 'fps'
local updateConnection: EventConnectionId
local renderConnection: EventConnectionId
local drawColor = Color(255, 255, 255)
local Module = {}

function Module.init()
    accumulatedSeconds = 0
    frameCount = 0
    label = "fps"
    updateConnection = game.Updating:Connect(function(dt: number)
        frameCount += 1
        accumulatedSeconds += dt
        if accumulatedSeconds >= 1 then
            label = `{frameCount} fps`
            frameCount = 0
            accumulatedSeconds = 0
        end
    end)
    renderConnection = game.Rendering:Connect(function()
        local OLD_DRAW_COLOR = Graphics.DrawColor
        Graphics.DrawColor = drawColor
        Graphics.DrawString(label, Matrix3.FromTranslation(Vector2(0, 0)))
        Graphics.DrawColor = OLD_DRAW_COLOR
    end)
end

function Module.deinit()
    game.Updating:Disconnect(updateConnection)
    game.Updating:Disconnect(renderConnection)
end

return Module
