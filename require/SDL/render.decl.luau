export type Module = {
    setDrawColor: (Coloru32) -> (),
    renderCopy: (SDL_Texture, dest: Recti?, src: Recti?) -> (),
    drawRect: ((Recti) ->()) & ((Rect) -> ()),
    fillRect: ((Recti) ->()) & ((Rect) -> ()),
}

return (assert(false, "file cannot be required")::never)::Module
