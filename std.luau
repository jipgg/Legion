--!strict
export type Array<Val> = {[number]: Val}
export type Map<Key, Val> = {[Key]: Val}
export type Connection = {
    id_: number,
    event_: any,
    disconnect: (Connection)->(),
}
local Connection: Connection = {}::never
(Connection::never).__index = Connection
function Connection:disconnect()
    self.event_.handlers_[self.id_] = nil
end
export type Event<Args...> = {
    init: ()-> Event<Args...>,
    deinit: (Event<Args...>) -> (),
    CURR_ID_: number,
    handlers_: Array<(Args...)->()>,
    connect: (Event<Args...>, (Args...)->()) -> Connection,
    fire: (Event<Args...>, Args...) -> (),
}
local Event: Event<...any> = {CURR_ID_ = 0}::never 
(Event::never).__index = Event;
function Event.init()
    return setmetatable({
        handlers_ = {},
    }::any, Event)
end
function Event:deinit()
    table.clear(self.handlers_)
end
function Event:fire(...)
    for i, handler in self.handlers_ do
        handler(...)
    end
end
function Event:connect(fn)
    self.CURR_ID_ += 1
    local id = self.CURR_ID_
    self.handlers_[id] = fn
    return setmetatable({id_ = id, event_ = self}::any, Connection)
end

export type Button = {
    box: Recti64,
    init: (fields: {box: Recti64?}) -> Button,
}
local Button: Button = { box=Recti64() }::never
(Button::never).__index = Button
function Button.init(fields)
    return setmetatable(fields::any, Button)
end

local run = {
}
function run.on_update(fn: (delta_s: number)->()): ()
    _G.__legion_update_fn = fn
end
function run.on_render(fn: ()->()): ()
    _G.__legion_render_fn = fn
end
function run.on_start(fn: ()->()): ()
    _G.__legion_start_fn = fn
end
function run.on_quit(fn: ()->()): ()
    _G.__legion_shutdown_fn = fn
end

return {
    Button = Button,
    Event = Event,
    run = run,
}
