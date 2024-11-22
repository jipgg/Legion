local M = {}
function M.print_this()
    print("this")
end
function M.exported_print_this()
    print("im exporting")
    M.print_this()
end
export type Module = {
    exported_print_this: ()->(),
}

setmetatable(M :: any, {
    __metatable = "locked",
    __newindex = function() error("module cannot be modified") end,
    __index = function() error("cannot assign a value to a module") end,
})
return M :: Module
