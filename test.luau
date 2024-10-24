--!strict
print("hello from luau")
local a = Vec2f(0, 123)
local clr = Coloru32(255, 255, 255, 255)
local entities = {}
a.x = 444
print(a, -a);
print(Recti64(10, 10, 100, 200))
print(Sizei32(10000).width)
type Part = {
    physical: Physical,
    update: (self: Part, delta: number) -> (),
    init: (self: Part) -> Part,
}
local function Part(fields: {physical: Physical?}): Part
    if not fields.physical then fields.physical = Physical() end
    return fields::Part
end
local entity = Part{}
entities.button = Clickable(Recti64(0, 0, 1000, 1000));
print("hmm")
entities.button:on_mouse_down(function(button: number, pos: Vec2i)
    print("yoooooooo")
end)
print("alive")
table.insert(entities, entity);
