print("hello from luau")
local tbl = {}
for i = 0, 10 do
    table.insert(tbl, i)
end
print(table.concat(tbl, ', '))
