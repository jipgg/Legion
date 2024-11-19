local Graphics = core.get_module("Graphics")
local GRender = Graphics.render
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
    M.updateConnection = core.during_update:connect(function(dt: number)
        M.frameCount += 1
        M.accumulatedSeconds += dt
        if M.accumulatedSeconds >= 1 then
            M.text = `{M.frameCount} fps`
            M.frameCount = 0
            M.accumulatedSeconds = 0
        end
    end)
    M.renderConnection = core.during_render:connect(function()
        local OLD_DRAW_COLOR = Graphics.color
        Graphics.color = M.drawColor
        GRender.text(M.text, Mat3.from_translation(Vec2(0, 0)))
        Graphics.color = OLD_DRAW_COLOR
    end)
end

function M.deinit()
    if M.updateConnection then
    end
    core.during_update:disconnect(assert(M.updateConnection))
    core.during_render:disconnect(assert(M.renderConnection))
end

return M :: Module
