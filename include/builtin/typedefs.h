#pragma once
#include "common/common.h"
#include "types.h"
#include <memory>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <blaze/Blaze.h>
namespace builtin {
using texture_t = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using font_t = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>;
struct rect_t {double x, y, w, h;};
using recti_t = SDL_Rect;
using color_t = SDL_Color;
using vec2i_t = common::vec2i;
using vec2d_t = common::vec2d;
using vec3d_t = blaze::StaticVector<double, 3, blaze::defaultTransposeFlag, blaze::aligned,
    blaze::unpadded /*not 100% sure why but this causes issues when turning ud back to blaze type if set padded*/>; 
using vec_t = blaze::DynamicVector<double>;
using physical_component_t = types::physical_component;
using mat3x3_t = common::mat3x3;
using mat3x2_t = blaze::StaticMatrix<double, 3, 2>;
}
