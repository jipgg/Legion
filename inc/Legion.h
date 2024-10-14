#include "common.h"
#include <SDL_main.h>
#include <iostream>
#include <filesystem>
#include <span>
#include <array>
#include <string_view>
#include <SDL_scancode.h>
#include <memory>
namespace fs = std::filesystem;
// forward declarations
struct _TTF_Font;
struct SDL_Texture;
struct SDL_Window;
struct lua_State;
//components
class Script {
    std::unique_ptr<lua_State, void(*)(lua_State*)> script_thread_;
public:
    Script(const fs::path& file);
    Script(std::string_view string);
    void load_string(std::string_view string);
    void load_file(const fs::path& filepath);
    void call(int argc = 0, int retc = 0);
};
class Font {
    _TTF_Font* font_;
public:
    Font(const fs::path& ttfpath, int ptsize) noexcept;
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
    Texture(const fs::path& path) noexcept;
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
//solvers
namespace solvers {
float effective_elasticity(float e1, float e2);
std::array<Vec2f, 2> velocities_after_collision(float  e1, float m1, const Vec2f& u1, float e2, float m2, const Vec2f& u2);
bool is_overlapping(const Recti64 &a, const Recti64 &b);
bool is_in_bounds(const Vec2i16& point, const Recti64& bounds);
}
//systems
void render_system(std::span<Renderable> components);
void update_system(std::span<Updatable> components, double delta_s);
void physics_system(std::span<Physical> components, double delta_s);
void player_input_system(const Playable& plr, Physical& phys, double delta_s);
namespace engine {
using Update_fn = void(*)(double delta_time);//maybe use std::function for these
using Render_fn = void(*)();
using Start_fn = void(*)();
using Shutdown_fn = void(*)();
struct Start_options {
    std::string window_name{"Legion"};
    Vec2i window_size{1240, 720};
    bool window_resizable{false};
    bool hardware_accelerated{true};
    bool vsync_enabled{true};
    Update_fn update_function{nullptr};
    Render_fn render_function{nullptr};
    Start_fn start_function{nullptr};
    Shutdown_fn shutdown_function{nullptr};
};
struct Game_interface {
    virtual ~Game_interface() = default;
    virtual Start_options config() = 0;
    virtual void start() = 0;
    virtual void update() = 0;
    virtual void teardown() = 0;
};
int bootstrap(Start_options opts = {});
namespace renderer {
void clear_frame();
void draw(const Recti64& rect);
void fill(const Recti64& rect);
void set_color(Coloru32 color);
}//renderer end
namespace core {
void start(Start_options opts = {});
void run();
void shutdown();
SDL_Window* get_window();// not preferrable, but need it for textures
lua_State* get_lua_state();
}//core end
}
template <class ...Ts>
void print(Ts&&...args) {
    ((std::cout << args << ' '), ...) << '\n';
}
template <class ...Ts>
void printerr(Ts&&...args) {
    std::cerr << "\033[31m";//red
    ((std::cerr << args << ' '), ...) << "\033[0m\n";
}
//struct defnitions
