--!strict
local Event = require 'Event'
local state = require 'state'
type Click_event = Event.Event<vec2d>
export type Button = {
    hit_area: recti64,
    clicked: Click_event,
    destroy: (Button) -> (),
    _mouse_released_cnnc: Event.Connection,
    _render_cnnc: Event.Connection,
    __index: Button,
    __mode: 'v',
    __type: 'Button',
}
local Button: Button = {
    __mode = 'v',
    __type = 'Button',
}::Button
Button.__index = Button

function Button:destroy()
    self._mouse_released_cnnc:disconnect()
    self._render_cnnc:disconnect()
    self.clicked:destroy()
end

local class = {}
function class.new(t: {hit_area: recti64?}): Button
    local self = t::Button
    self.hit_area = t.hit_area or recti64()
    self.clicked = Event.create()::Click_event
    self._mouse_released_cnnc = state.mouse_released:connect(function(button: mouse_button_type, pos: vec2d)
        if builtin.point_in_rect(pos, self.hit_area) then
            self.clicked:fire(pos)
        end
    end)
    self._render_cnnc = state.rendering:connect(function()
        renderer.fill(self.hit_area)
    end)
    return setmetatable(self, Button)::any
end
return class
