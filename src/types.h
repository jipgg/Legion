#pragma once
#include "common.h"
#include <filesystem>
#include <SDL_scancode.h>
#include <string_view>
#include "event.h"
#include <SDL_mouse.h>
struct _TTF_Font;
struct SDL_Texture;
struct lua_State;
namespace types {
// forward declarations
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
    common::Sizei32 src_size_;
public:
    Texture(const std::filesystem::path& path) noexcept;
    Texture(const Texture& a) = delete;
    Texture& operator=(const Texture& a) = delete;
    Texture(Texture&& a) noexcept;
    Texture& operator=(Texture&& a) noexcept;
    ~Texture() noexcept;
    common::Sizei32 src_size() const;
};
struct Playable {
    float jump_power, walk_speed;
    SDL_Scancode up, down, left, right, jump;
};
struct Clickable {
    enum class Button {
        Left = SDL_BUTTON_LEFT,
        Right = SDL_BUTTON_RIGHT,
        Middle = SDL_BUTTON_MIDDLE,
    };
    common::Recti64 hit;
    using Mouse_event = event::Async<Button, common::Vec2i>;
    Mouse_event on_mouse_up;
    Mouse_event on_mouse_down;
};
struct Renderable {
    std::function<void()> render_fn;
};
struct Updatable {
    std::function<void(double delta_s)> update_fn;
};
struct Physical {
    common::Vec2d position{0, 0};
    common::Vec2d velocity{0, 0};
    common::Vec2d acceleration{0, 0};
    bool welded{true};//maybe put these in a bitset
    bool falling{true};
    bool obstructed{true};
    float elasticity_coeff{.3f};
    float friction_coeff{.3f};
    float mass{1.f};
    common::Vec2d size{100, 100};
    common::Recti64 bounds() const {
        return {
            static_cast<int16_t>(position.at(0)),
            static_cast<int16_t>(position.at(1)),
            static_cast<int16_t>(size.at(0)),
            static_cast<int16_t>(size.at(1)),
        };
    }
    std::array<common::Vec2d, 4> corner_points() const {
        using V2 = common::Vec2d;
        V2 left = position + V2{0, size.at(1) / 2.f};
        V2 right = position + V2{size.at(0), size.at(1) / 2.f};
        V2 top = position + V2{size.at(0) / 2.f, 0};
        V2 bottom = position + V2{size.at(0) / 2.0, size.at(1)};
        return {left, right, top, bottom};
    }
};
}
