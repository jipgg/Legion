local Library = {}

function Library.ireverse<T>(t: {T})
    local i = #t + 1
    return function(): (number?, T?)
        i -= 1
        if i > 0 then
            return i, t[i]
        else
            return nil
        end
    end
end

export type Predicate<T> = (e: T) -> boolean
function Library.ifilter<T>(t: {T}, pred: (e: T) -> boolean)
    local i = 0
    return function(): (number?, T)
        i += 1
        local v = t[i]
        while i <= #t and not pred(v) do
            i += 1
            v = t[i]
        end
        if i <= #t then
            return i, v
        else
            return nil, nil::never
        end
    end
end
function Library.filter<T>(t: { [any]: T }, pred: (e: T) -> boolean)
    local key, value
    return function(): (any, T?)
        repeat
            key, value = next(t, key)
        until key == nil or pred(value)
        if key ~= nil then
            return key, value
        else
            return nil, nil::never
        end
    end
end

return Library
