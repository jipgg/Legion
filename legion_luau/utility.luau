local module = {}
export type Pair<T, U> = {first: T, second: U}
function module.make_pair<T, U>(first: T, second: U): Pair<T, U>
    return {first = first, second = second}
end

return module
