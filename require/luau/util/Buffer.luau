--!strict
export type Type = 'i8'|'i16'|'i32'|'u8'|'u16'|'u32'|'f32'|'f64'

local Buffer = {}
Buffer.__index = Buffer
export type self = typeof(setmetatable({
    size = 0,
    capacity = 0,
    data = buffer.create(0)}, Buffer))

function Buffer.create(bytesize: number)
    return setmetatable({
        capacity = bytesize,
        size = 0,
        data = buffer.create(bytesize)
    }, Buffer)
end


function Buffer:push(t: Type, ...: number)
    if t == 'i8' then
        for i = 1, select('#', ...) do
            buffer.writei8(self.data, self.size, select(i, ...))
            self.size += 1
        end
    elseif t == 'i16' then
        for i = 1, select('#', ...) do
            buffer.writei16(self.data, self.size, select(i, ...))
            self.size += 2
        end
    elseif t == 'i32' then
        for i = 1, select('#', ...) do
            buffer.writei32(self.data, self.size, select(i, ...))
            self.size += 4
        end
    elseif t == 'u8' then
        for i = 1, select('#', ...) do
            buffer.writeu8(self.data, self.size, select(i, ...))
            self.size += 1
        end
    elseif t == 'u16' then
        for i = 1, select('#', ...) do
            buffer.writeu16(self.data, self.size, select(i, ...))
            self.size += 2
        end
    elseif t == 'u32' then
        for i = 1, select('#', ...) do
            buffer.writeu32(self.data, self.size, select(i, ...))
            self.size += 4
        end
    elseif t == 'f32' then
        for i = 1, select('#', ...) do
            buffer.writef32(self.data, self.size, select(i, ...))
            self.size += 4
        end
    elseif t == 'f64' then
        for i = 1, select('#', ...) do
            buffer.writef64(self.data, self.size, select(i, ...))
            self.size += 8
        end
    end
end

return Buffer 
