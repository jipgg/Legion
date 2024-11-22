#include "gui.h"
#include "engine.h"
#include <SDL.h>
#include "common.h"
#include "util.h"
using engine::renderer;

namespace gui {
void Frame::render(const SDL_Rect& parent_rect) {
    SDL_SetRenderDrawBlendMode(renderer(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(
        renderer(),
        background_color.r,
        background_color.g,
        background_color.b,
        transparencyu8()
    );
    const SDL_Rect my_rect = to_screen_rect(parent_rect); 
    SDL_RenderFillRect(renderer(), &my_rect);
    SDL_SetRenderDrawColor(renderer(), 0, 0, 255, 0xff);
    util::draw_rect_with_line_width(my_rect, 5);
}
}
