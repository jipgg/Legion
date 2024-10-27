--!strict
local Event = require("Event")
local state = require("state")
type Click_event = Event.Event<Vec2>
export type Button = {
    hit_area: Rect,
    clicked: Click_event,
    destroy: (Button) -> (),
    _mouse_released_cnnc: Event.Connection,
    _render_cnnc: Event.Connection,
    __index: Button,
    __mode: Weak_v,
}
local Button: Button = {}::never
Button.__index = Button

function Button:destroy()
    self._mouse_released_cnnc:disconnect()
    self._render_cnnc:disconnect()
    self.clicked:destroy()
end

local class = {}
function class.new(t: {hit_area: Rect?}): Button
    local self = t::Button
    self.hit_area = t.hit_area or Rect()
    self.clicked = Event.create()::Click_event
    self._mouse_released_cnnc = state.mouse_released:connect(function(button: Mousebutton, pos: Vec2)
        if builtin.point_in_rect(pos, self.hit_area) then
            self.clicked:fire(pos)
        end
    end)
    self._render_cnnc = state.rendering:connect(function()
        renderer.fill(self.hit_area)
    end)
    return setmetatable(self, Button)::never
end
return class
