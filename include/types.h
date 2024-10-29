#pragma once
#include "common/common.h"
#include <filesystem>
#include <SDL_scancode.h>
#include <string_view>
#include "event.h"
#include <SDL_render.h>
#include <SDL_mouse.h>
struct _TTF_Font;
struct SDL_Texture;
struct lua_State;
namespace types {
// forward declarations
//components
class script {
    std::unique_ptr<lua_State, void(*)(lua_State*)> script_thread_;
public:
    script(const std::filesystem::path& file);
    script(std::string_view string);
    void load_string(std::string_view string);
    void load_file(const std::filesystem::path& filepath);
    void call(int argc = 0, int retc = 0);
};
class font {
    _TTF_Font* font_;
public:
    font(const std::filesystem::path& ttfpath, int ptsize) noexcept;
    font(const font& a) = delete;
    font& operator=(const font& a) = delete;
    font(font&& a) noexcept;
    font& operator=(font&& a) noexcept;
    ~font() noexcept;
};
struct texture {
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> data;
    common::vec2i src_size;
    std::filesystem::path path;
    texture(const std::filesystem::path& path);
};
struct playable {
    float jump_power, walk_speed;
    SDL_Scancode up, down, left, right, jump;
};
struct physical_component {
    common::vec2d position{0, 0};
    common::vec2d velocity{0, 0};
    common::vec2d acceleration{0, 0};
    bool welded{true};//maybe put these in a bitset
    bool falling{true};
    bool obstructed{true};
    float elasticity_coeff{.3f};
    float friction_coeff{.3f};
    float mass{1.f};
    common::vec2d size{100, 100};
    common::recti64 bounds() const {
        return {
            static_cast<int16_t>(position.at(0)),
            static_cast<int16_t>(position.at(1)),
            static_cast<int16_t>(size.at(0)),
            static_cast<int16_t>(size.at(1)),
        };
    }
    std::array<common::vec2d, 4> corner_points() const {
        using v2 = common::vec2d;
        v2 left = position + v2{0, size.at(1) / 2.f};
        v2 right = position + v2{size.at(0), size.at(1) / 2.f};
        v2 top = position + v2{size.at(0) / 2.f, 0};
        v2 bottom = position + v2{size.at(0) / 2.0, size.at(1)};
        return {left, right, top, bottom};
    }
};
}
