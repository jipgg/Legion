local Graphics = core.GetModule("Graphics")
local GRender = Graphics.Render
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
    M.updateConnection = core.DuringUpdate:Connect(function(dt: number)
        M.frameCount += 1
        M.accumulatedSeconds += dt
        if M.accumulatedSeconds >= 1 then
            M.text = `{M.frameCount} fps`
            M.frameCount = 0
            M.accumulatedSeconds = 0
        end
    end)
    M.renderConnection = core.DuringRender:Connect(function()
        local OLD_DRAW_COLOR = Graphics.Color
        Graphics.Color = M.drawColor
        GRender.Text(M.text, Matrix3.FromTranslation(Vector2(0, 0)))
        Graphics.Color = OLD_DRAW_COLOR
    end)
end

function M.deinit()
    if M.updateConnection then
    end
    core.DuringUpdate:Disconnect(assert(M.updateConnection))
    core.DuringRender:Disconnect(assert(M.renderConnection))
end

return M :: Module
