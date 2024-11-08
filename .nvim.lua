require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp", "lsp", "--definitions=resources/type_definitions.d.luau"},
    settings = {
        ["luau-lsp"] = {
            platform = {
                type = "standard",
            },
            require = {
                mode = "relativeToFile",
                directoryAliases = {
                    ["@tests"] = "./tests/",
                    ["@builtin"] = "./resources/luau_library/",
                },
            },
        }
    }
}
