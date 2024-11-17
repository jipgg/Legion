local testVector2 = Vector2(100, 100)
testVector2:DotProduct(testVector2)
assert(testVector2:ToUnitVector():Length() == 1)
testVector2:Abs()
local dummy: any = testVector2 + testVector2
dummy = testVector2 - testVector2
dummy = testVector2:ToUnitVector():DotProduct(Vector2(1, 0))
print('all tests passed.')
