require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp", "lsp", "--definitions=typedefs.d.luau"},
    settings = {
        ["luau-lsp"] = {
            platform = {
                type = "standard",
            },
            require = {
                mode = "relativeToFile",
                --directoryAliases = {["@Legion"] = "./require/"},
            },
        }
    }
}
