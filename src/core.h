#ifndef LEGION_CORE_H
#define LEGION_CORE_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <lua.h>
#include <lualib.h>
#include <memory>
namespace luau {
struct State {
    std::unique_ptr<lua_State, decltype(&lua_close)> state;
    State(lua_State* l = nullptr): state(l ? l : luaL_newstate(), lua_close) {}
    operator lua_State*() const {return state.get();}
};
}
namespace sdl {
using Event = SDL_Event;
using Rect = SDL_Rect;
using Point = SDL_Point;
struct Line {SDL_Point t0, t1;};
using Color = SDL_Color;
struct Size {int w, h;};
using Rectf = SDL_FRect;
namespace __impl {
template <class T>
using SDL_dtor = void(*)(T*);
template <class T>
using SDL_unique_ptr = std::unique_ptr<T, SDL_dtor<T>>;
}
template <class T>
class Resource {
    __impl::SDL_unique_ptr<T> _resource;
    template <class U>
    friend void destroy(Resource<U>);
public:
    Resource(T* obj, __impl::SDL_dtor<T> dtor): _resource(obj, dtor) {}
    virtual ~Resource() = default;
    T * get() const {return _resource.get();}
    operator T*() const {return _resource.get();}
    void destroy() {_resource.reset();}
};
struct Window: public Resource<SDL_Window> {
    Window(SDL_Window* window): Resource(window, SDL_DestroyWindow) {}
};
struct Renderer: public Resource<SDL_Renderer> {
    Renderer(SDL_Renderer* window): Resource(window, SDL_DestroyRenderer) {}
};
struct Surface: public Resource<SDL_Surface> {
    Surface(SDL_Surface* surface): Resource(surface, SDL_FreeSurface) {}
};
struct Texture: public Resource<SDL_Texture> {
    Texture(SDL_Texture* texture): Resource(texture, SDL_DestroyTexture) {}
};
struct Font: public Resource<TTF_Font> {
    Font(TTF_Font* font): Resource(font, TTF_CloseFont) {}
};
inline void set_draw_color(Renderer& rnd, Color col) {
    SDL_SetRenderDrawColor(rnd, col.r, col.g, col.b, col.a);
}
template<class T>
inline void destroy(Resource<T>& res) {
    res.destroy();
}
}/*SDL*/
#endif
