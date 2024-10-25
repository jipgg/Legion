require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp", "lsp", "--definitions=builtin.d.luau"},
    settings = {
        ["luau-lsp"] = {
            platform = {
                type = "standard",
            },
            require = {
                mode = "relativeToFile",
                --directoryAliases = {legion = "./legion_luau/"},
            },
        }
    }
}
