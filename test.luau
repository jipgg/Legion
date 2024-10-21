--!strict
print("hello from luau")
local a = V2(0, 123)
a.x = 444
print(a, -a);
local tbl = {}
for i = 0, 10 do
    table.insert(tbl, i)
end
print(table.concat(tbl, ', '))
