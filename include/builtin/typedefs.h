#pragma once
#include "common/common.h"
#include "types.h"
#include <memory>
#include <SDL_render.h>
#include <SDL_ttf.h>
namespace builtin {
using texture_t = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using font_t = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>;
using point_t = SDL_Point;
using rect_t = SDL_Rect;
using coloru32_t = common::coloru32;
using recti64_t = common::recti64;
using vec2i_t = common::vec2i;
using vec2d_t = common::vec2d;
using physical_component_t = types::physical_component;
}
