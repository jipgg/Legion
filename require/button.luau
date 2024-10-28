--!strict
local event = require 'event'
local state = require 'state'
type click_event = event.event<vec2d>
export type button = {
    hit_area: recti64,
    clicked: click_event,
    destroy: (button) -> (),
    _mouse_released_cnnc: event.connection,
    _render_cnnc: event.connection,
    __index: button,
    __mode: 'v',
    __type: 'button',
}
local button: button = {
    __mode = 'v',
    __type = 'button',
}::button
button.__index = button

function button:destroy()
    self._mouse_released_cnnc:disconnect()
    self._render_cnnc:disconnect()
    self.clicked:destroy()
end

function construct(t: {hit_area: recti64?}): button
    local self = t::button
    self.hit_area = t.hit_area or recti64()
    self.clicked = event()::click_event
    self._mouse_released_cnnc = state.mouse_released:connect(function(button: mouse_button_type, pos: vec2d)
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
