type Module = {
    openFont: (file: string, ptsize: number) -> TTF_Font?,
    loadImage: (file: string) -> (SDL_Texture?, number, number),
    loadText: (font: TTF_Font, text: string, fg_or_black: Coloru32?) -> (SDL_Texture?, number, number),
    render: typeof(require 'render.decl'),
}
return _G.__builtin_import_sdl()::Module
