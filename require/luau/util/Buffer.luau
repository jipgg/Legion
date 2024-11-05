--!strict
export type Type = 'i8'|'i16'|'i32'|'u8'|'u16'|'u32'|'f32'|'f64'

local Buffer = {}
Buffer.__index = Buffer
export type self = typeof(setmetatable({
    size = 0,
    capacity = 0,
    data = buffer.create(0)}, Buffer))

local function construct(bytesize: number)
    return setmetatable({
        capacity = bytesize,
        size = 0,
        data = buffer.create(bytesize)
    }, Buffer)
end
export type Buffer = typeof(construct(1))
type TypeSwitch = {
    i8: (self, ...number)->(),
    i16: (self, ...number)->(),
    i32: (self, ...number)->(),
    u8: (self, ...number)->(),
    u16: (self, ...number)->(),
    u32: (self, ...number)->(),
    f32: (self, ...number)->(),
    f64: (self, ...number)->(),
}
local pushImpl: TypeSwitch = {
    i8 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writei8(self.data, self.size, select(i, ...))
            self.size += 1
        end
    end,
    i16 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writei16(self.data, self.size, select(i, ...))
            self.size += 2
        end
    end,
    i32 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writei32(self.data, self.size, select(i, ...))
            self.size += 4
        end
    end,
    u8 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writeu8(self.data, self.size, select(i, ...))
            self.size += 1
        end
    end,
    u16 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writeu16(self.data, self.size, select(i, ...))
            self.size += 2
        end
    end,
    u32 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writeu32(self.data, self.size, select(i, ...))
            self.size += 4
        end
    end,
    f32 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writef32(self.data, self.size, select(i, ...))
            self.size += 4
        end
    end,
    f64 = function(self, ...)
        for i = 1, select('#', ...) do
            buffer.writef64(self.data, self.size, select(i, ...))
            self.size += 8
        end
    end,
}

function Buffer:push(t: Type, ...: number)
    pushImpl[t](self, ...)
end

return construct

