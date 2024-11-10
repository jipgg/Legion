local task = {}

function task.spawn<Ts...>(fn: (Ts...)->(), ...: Ts...)
    local cor = coroutine.create(function(...) fn(...) end)
    coroutine.resume(cor)
end

function task.wait(count: number)
    local thread = coroutine.running()
    if thread then
        print("is coroutine")
        coroutine.yield()
    else
        print("is main thread")
    end
end

return task
