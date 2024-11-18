require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp",
        "lsp",
        "--definitions=resources/builtin_defs.d.luau",
        "--docs=resources/builtin_docs.json",
    },
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
                    ["@example"] = "./example/",
                },
            },
        }
    }
}
