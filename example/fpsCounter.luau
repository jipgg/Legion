local Graphics = game.GetModule("Graphics")
local M = {
    accumulatedSeconds = 0,
    frameCount = 0,
    text = 'fps',
    updateConnection = nil :: EventConnectionId?,
    renderConnection = nil :: EventConnectionId?,
    drawColor = Color(255, 255, 255)
}
export type Module = {--public interface
    drawColor: Color,
    init: ()->(),
    deinit: ()->(),
}

function M.init()
    M.accumulatedSeconds = 0
    M.frameCount = 0
    M.text = "fps"
    M.updateConnection = game.Updating:Connect(function(dt: number)
        M.frameCount += 1
        M.accumulatedSeconds += dt
        if M.accumulatedSeconds >= 1 then
            M.label = `{M.frameCount} fps`
            M.frameCount = 0
            M.accumulatedSeconds = 0
        end
    end)
    M.renderConnection = game.Rendering:Connect(function()
        local OLD_DRAW_COLOR = Graphics.DrawColor
        Graphics.DrawColor = M.drawColor
        Graphics.DrawString(M.label, Matrix3.FromTranslation(Vector2(0, 0)))
        Graphics.DrawColor = OLD_DRAW_COLOR
    end)
end

function M.deinit()
    if M.updateConnection then
    end
    game.Updating:Disconnect(assert(M.updateConnection))
    game.Updating:Disconnect(assert(M.renderConnection))
end

return M :: Module
