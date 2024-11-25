local Graphics = waw.get_builtin_module('Graphics')
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
    M.updateConnection = waw.onUpdate:connect(function(dt: number)
        M.frameCount += 1
        M.accumulatedSeconds += dt
        if M.accumulatedSeconds >= 1 then
            M.text = `{M.frameCount} fps`
            M.frameCount = 0
            M.accumulatedSeconds = 0
        end
    end)
    M.renderConnection = waw.onRender:connect(function()
        local OLD_DRAW_COLOR = Graphics.color
        Graphics.color = M.drawColor
        Graphics.render.text(M.text, Mat3.from_translation(Vec2(0, 0)))
        Graphics.color = OLD_DRAW_COLOR
    end)
end

function M.deinit()
    if M.updateConnection then
    end
    waw.onUpdate:disconnect(assert(M.updateConnection))
    waw.onRender:disconnect(assert(M.renderConnection))
end

return M :: Module
