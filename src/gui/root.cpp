#include "gui.h"
#include "engine.h"
using RenderCache = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
static std::array<RenderCache, 2> buffers {RenderCache{nullptr, SDL_DestroyTexture}, RenderCache{nullptr, SDL_DestroyTexture}};
static bool is_dirty{true};
static bool is_buffer_size_dirty{true};
static int active_buffer{0};
static void init_buffer(int idx, const Vec2i& size) {
    assert(idx >= 0 and idx < buffers.size());
    buffers[idx].reset(SDL_CreateTexture(
        engine::renderer(),
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        size[0], size[1]
    ));
}
static void swap_buffer() {
    ++active_buffer %= buffers.size(); 
}
static SDL_Texture* back_buffer() {
    return buffers[(active_buffer + 1) % buffers.size()].get();
}
static SDL_Texture* front_buffer() {
    return buffers[active_buffer].get();
}
static void resize_buffers(const Vec2i& size) {
    init_buffer(0, size);
    init_buffer(1, size);
}
namespace gui {
namespace root {
void set_dirty() {
    is_dirty = true;
}
void handle_events(const SDL_Event* const e) {
    switch(e->type) {
        case SDL_MOUSEBUTTONDOWN:
            break;
        case SDL_MOUSEBUTTONUP:
            break;
        case SDL_WINDOWEVENT:
            if (SDL_WINDOWEVENT_SIZE_CHANGED) {
                resize_buffers(engine::window_size());
                set_dirty();
            }
            break;
    }
}
void init() {
    const Vec2i window_size = engine::window_size();
    init_buffer(0, window_size);
    init_buffer(0, window_size);
}
void redraw() {
    SDL_Rect window{0, 0, 0, 0};
    SDL_SetRenderTarget(engine::renderer(), back_buffer());
    SDL_GetWindowSize(engine::window(), &window.w, &window.h);
    for (const auto& elem : elements) {
        elem->render(window);
        elem->render_descendants(window);
    }
    SDL_SetRenderTarget(engine::renderer(), nullptr);
    swap_buffer();
}
void render() {
    if (is_dirty) {
        redraw();
        is_dirty = false;
    }
    SDL_RenderCopy(engine::renderer(), front_buffer(), nullptr, nullptr);
}
}
}
