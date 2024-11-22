#pragma once
#include <memory>
#include <vector>
#include <span>
#include <SDL.h>
#include "structs.h"
namespace gui {
struct UDim2 {
    int x_offset{};
    int y_offset{};
    double x_scale{};
    double y_scale{};
};
constexpr static void apply_udim2(const UDim2& dim, int& x, int& y) {
    x = static_cast<int>(x * dim.x_scale) + dim.x_offset;
    y = static_cast<int>(y * dim.y_scale) + dim.y_offset;
}
class Element: public std::enable_shared_from_this<Element> {
public:
    using Shared = std::shared_ptr<Element>;
    using Weak = std::weak_ptr<Element>;
    UDim2 position{};
    UDim2 size{.x_scale = 1, .y_scale = 1};
    void set_parent(const Shared& new_parent);
    std::span<const Shared> children() const;
    Weak parent() const;
    void remove_child(Element* child);
    void adopt_child(Shared&& child);
    virtual void render(const SDL_Rect& parent_rect) = 0;
    void render_descendants(const SDL_Rect& parent_rect) const;
    constexpr SDL_Rect to_screen_rect(const SDL_Rect& parent) const {
        SDL_Rect ret{parent};
        apply_udim2(position, ret.x, ret.y);
        apply_udim2(size, ret.w, ret.h);
        return ret;
    }
    inline static bool buffer_dirty_flag{true}; 
private:
    Weak parent_;
    std::vector<Shared> children_;
};
namespace root {
inline static std::vector<Element::Shared> elements;
void redraw();
void render();
void init();
void handle_event(const SDL_Event* const e);
void set_dirty();
}
class Frame: public Element {
public:
    Color3u8 background_color{Color3u8::preset(Color3u8::White)};
    double transparency{0.};
    int border_pixel{1};
    Color3u8 border_color{Color3u8::preset(Color3u8::Black)};
    virtual void render(const SDL_Rect& parent_rect) override;
    constexpr uint8_t transparencyu8() {
        return static_cast<uint8_t>(255 * (1 - transparency));
    }
};
}
