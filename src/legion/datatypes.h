#pragma once
#include "common.h"
#include <filesystem>
#include <SDL_scancode.h>
#include <string_view>
using Entity_t = std::uint32_t;
// forward declarations
struct _TTF_Font;
struct SDL_Texture;
struct lua_State;
//components
class Script {
    std::unique_ptr<lua_State, void(*)(lua_State*)> script_thread_;
public:
    Script(const std::filesystem::path& file);
    Script(std::string_view string);
    void load_string(std::string_view string);
    void load_file(const std::filesystem::path& filepath);
    void call(int argc = 0, int retc = 0);
};
class Font {
    _TTF_Font* font_;
public:
    Font(const std::filesystem::path& ttfpath, int ptsize) noexcept;
    Font(const Font& a) = delete;
    Font& operator=(const Font& a) = delete;
    Font(Font&& a) noexcept;
    Font& operator=(Font&& a) noexcept;
    ~Font() noexcept;
};
class Texture {
    SDL_Texture* texture_;
    Sizei32 src_size_;
public:
    Texture(const std::filesystem::path& path) noexcept;
    Texture(const Texture& a) = delete;
    Texture& operator=(const Texture& a) = delete;
    Texture(Texture&& a) noexcept;
    Texture& operator=(Texture&& a) noexcept;
    ~Texture() noexcept;
    Sizei32 src_size() const;
};
struct Playable {
    float jump_power, walk_speed;
    SDL_Scancode up, down, left, right, jump;
};
struct Renderable {
    std::function<void()> render_fn;
};
struct Updatable {
    std::function<void(double delta_s)> update_fn;
};
struct Physical {
    Vec2f position{0, 0};
    Vec2f velocity{0, 0};
    Vec2f acceleration{0, 0};
    bool welded{true};//maybe put these in a bitset
    bool falling{true};
    bool obstructed{true};
    float elasticity_coeff{.3f};
    float friction_coeff{.3f};
    float mass{1.f};
    Sizei32 size{100, 100};
    Recti64 bounds() const {
        return {
            static_cast<int16_t>(position.at(0)),
            static_cast<int16_t>(position.at(1)),
            size.width(), size.height()
        };
    }
    std::array<Vec2f, 4> corner_points() const {
    Vec2f left = position + Vec2f{0, size.height() / 2.f};
    Vec2f right = position + Vec2f{static_cast<float>(size.width()), size.height() / 2.f};
    Vec2f top = position + Vec2f {size.width() / 2.f, 0};
    Vec2f bottom = position + Vec2f{size.width() / 2.f, static_cast<float>(size.height())};
    return {left, right, top, bottom};
}
};
