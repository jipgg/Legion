type V2 = (number, number) -> unknown
print("hello from luau")
local a = V2(0, 100)
print(a:unit())
local tbl = {}
for i = 0, 10 do
    table.insert(tbl, i)
end
print(table.concat(tbl, ', '))
