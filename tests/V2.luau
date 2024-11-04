local Test = {}
function Test.run()
    local a = V2(100, 100)
    local b = V2(10, -100)
    assert(a + a == V2(200, 200))
    assert(a + b == b + a, "__add is not complimentary")
    assert(a - b == V2(90, 200))
end

return Test
