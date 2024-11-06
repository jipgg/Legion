require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp", "lsp", "--definitions=globals.d.luau", "--docs=api-docs.json"},
    settings = {
        ["luau-lsp"] = {
            platform = {
                type = "standard",
            },
            require = {
                mode = "relativeToFile",
                directoryAliases = {
                    ["@tests"] = "./tests/",
                    ["@legion"] = "./require/",
                },
            },
        }
    }
}
