local function test_iter()
    local iter = require '@legion/iterator'
    local tbl = {6, 5, 4, 3, 2, 1}
    local function fn(e: number): boolean
        return e % 2 == 0
    end
    print("ifilter:")
    for v in iter.ifilter(tbl, fn) do
        print(`[*]: {v}`)
    end
    print("filter:")
    for v in iter.filter(tbl, fn) do
        print(`[*]: {v}`)
    end

    print('values:')
    for v in iter.elements(tbl) do
        print(`[*]: {v}`)
    end
    print('imap:')
    for v1, v2 in iter.imap(tbl, tbl) do
        assert(v1 == v2)
        print(`{v1} = {v2}`)
    end
end

test_iter()
print('all tests passed.')
