--!strict
local event = require 'event'
local state = require 'state'
type click_event = event.event<Vec2d>
export type Button = {
    hit_area: Recti64,
    clicked: click_event,
    destroy: (Button) -> (),
    _mouse_released_cnnc: event.connection,
    _render_cnnc: event.connection,
    __index: Button,
    __mode: 'v',
    __type: 'button',
}
local button: Button = {
    __mode = 'v',
    __type = 'button',
}::Button
button.__index = button

function button:destroy()
    self._mouse_released_cnnc:disconnect()
    self._render_cnnc:disconnect()
    self.clicked:destroy()
end

function construct(t: {hit_area: Recti64?}): Button
    local self = t::Button
    self.hit_area = t.hit_area or Recti64()
    self.clicked = event()
    self._mouse_released_cnnc = state.mouse_released:connect(function(button: mouse_button_type, pos: Vec2d)
        if builtin.point_in_rect(pos, self.hit_area) then
            self.clicked:fire(pos)
        end
    end)
    self._render_cnnc = state.rendering:connect(function()
        renderer.fill(self.hit_area)
    end)
    return setmetatable(self, button)::any
end
return construct
