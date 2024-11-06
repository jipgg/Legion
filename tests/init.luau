local it = require '@std/iterator'
local tbl = {6, 5, 4, 3, 2, 1}

local function fn(e: number): boolean
    return e % 2 == 0
end
print("ifilter:")
for i, v in it.ifilter(tbl, fn) do
    print(`[{i}]: {v}`)
end

print("filter:")
for i, v in it.filter(tbl, fn) do
    print(`[{i}]: {v}`)
end

print('all tests passed.')
