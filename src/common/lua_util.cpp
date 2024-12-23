#include "lua_util.h"
#include <unordered_map>
std::optional<std::string> resolve_path_type(lua_State* L, int i) {
    if (is_type<std::filesystem::path>(L, i)) {
        return std::make_optional(check<std::filesystem::path>(L, i).string());
    } else if (lua_isstring(L, i)) {
        return std::make_optional(luaL_checkstring(L, i));
    } else return std::nullopt;
}
static constexpr auto q = "Q", w = "W", e = "E", r = "R",
    t = "T", y = "Y", i = "I", o = "O", p = "P", a = "A",
    s = "S", d = "D", f = "F", g = "G", h = "H", j = "J",
    k = "K", l = "L", z = "Z", x = "X", c = "C", v = "V",
    b = "B", n = "N", m = "M", space = "Space", lctrl = "Left Control",
    lalt = "Left Alt", rctrl = "Right Control", ralt = "Right Alt", one = "1",
    two = "2", three = "3", four = "4", five = "5", six = "6",
    seven = "7", eight = "8", nine = "9", zero = "0",
    f1 = "F1", f2 = "F2", f3 = "F3", f4 = "F4", f5 = "F5",
    f6 = "F6", f7 = "F7", f8 = "F8", f9 = "F9", f10 = "F10",
    f11 = "F11", f12 = "F12", tab = "Tab", left = "Arrow Left", right = "Arrow Right",
    up = "Arrow Up", down = "Arrow Down", lshift = "Left Shift", rshift = "Right Shift",
    enter = "Enter", backslash = "Backslash", slash = "Slash", comma = "Comma",
    period = "Period", minus = "Minus", equals = "Equals", backspace = "Backspace",
    esc = "Escape";
static constexpr auto nullkey = "Undefined";
const char* scancode_to_string(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_Q: return q;
        case SDL_SCANCODE_W: return w;
        case SDL_SCANCODE_E: return e;
        case SDL_SCANCODE_R: return r;
        case SDL_SCANCODE_T: return t;
        case SDL_SCANCODE_Y: return y;
        case SDL_SCANCODE_I: return i;
        case SDL_SCANCODE_O: return o;
        case SDL_SCANCODE_P: return p;
        case SDL_SCANCODE_A: return a;
        case SDL_SCANCODE_S: return s;
        case SDL_SCANCODE_D: return d;
        case SDL_SCANCODE_F: return f;
        case SDL_SCANCODE_G: return g;
        case SDL_SCANCODE_H: return h;
        case SDL_SCANCODE_J: return j;
        case SDL_SCANCODE_K: return k;
        case SDL_SCANCODE_L: return l;
        case SDL_SCANCODE_Z: return z;
        case SDL_SCANCODE_X: return x;
        case SDL_SCANCODE_C: return c;
        case SDL_SCANCODE_V: return v;
        case SDL_SCANCODE_B: return b;
        case SDL_SCANCODE_N: return n;
        case SDL_SCANCODE_M: return m;
        case SDL_SCANCODE_SPACE: return space;
        case SDL_SCANCODE_LCTRL: return lctrl;
        case SDL_SCANCODE_LALT: return lalt;
        case SDL_SCANCODE_RCTRL: return rctrl;
        case SDL_SCANCODE_RALT: return ralt;
        case SDL_SCANCODE_1: return one;
        case SDL_SCANCODE_2: return two;
        case SDL_SCANCODE_3: return three;
        case SDL_SCANCODE_4: return four;
        case SDL_SCANCODE_5: return five;
        case SDL_SCANCODE_6: return six;
        case SDL_SCANCODE_7: return seven;
        case SDL_SCANCODE_8: return eight;
        case SDL_SCANCODE_9: return nine;
        case SDL_SCANCODE_0: return zero;
        case SDL_SCANCODE_F1: return f1;
        case SDL_SCANCODE_F2: return f2;
        case SDL_SCANCODE_F3: return f3;
        case SDL_SCANCODE_F4: return f4;
        case SDL_SCANCODE_F5: return f5;
        case SDL_SCANCODE_F6: return f6;
        case SDL_SCANCODE_F7: return f7;
        case SDL_SCANCODE_F8: return f8;
        case SDL_SCANCODE_F9: return f9;
        case SDL_SCANCODE_F10: return f10;
        case SDL_SCANCODE_F11: return f11;
        case SDL_SCANCODE_F12: return f12;
        case SDL_SCANCODE_TAB: return tab;
        case SDL_SCANCODE_LEFT: return left;
        case SDL_SCANCODE_RIGHT: return right;
        case SDL_SCANCODE_UP: return up;
        case SDL_SCANCODE_DOWN: return down;
        case SDL_SCANCODE_LSHIFT: return lshift;
        case SDL_SCANCODE_RSHIFT: return rshift;
        case SDL_SCANCODE_RETURN: return enter;
        case SDL_SCANCODE_BACKSPACE: return backspace;
        case SDL_SCANCODE_SLASH: return slash;
        case SDL_SCANCODE_BACKSLASH: return backslash;
        case SDL_SCANCODE_COMMA: return comma;
        case SDL_SCANCODE_PERIOD: return period;
        case SDL_SCANCODE_MINUS: return minus;
        case SDL_SCANCODE_EQUALS: return equals;
        case SDL_SCANCODE_ESCAPE: return esc;
        default: return nullkey;
    }
}
static std::unordered_map<std::string_view, SDL_Scancode> mappings {
    {q, SDL_SCANCODE_Q}, {w, SDL_SCANCODE_W}, {e, SDL_SCANCODE_E},
    {r, SDL_SCANCODE_R}, {t, SDL_SCANCODE_T}, {y, SDL_SCANCODE_Y},
    {i, SDL_SCANCODE_I}, {o, SDL_SCANCODE_O}, {p, SDL_SCANCODE_P},
    {a, SDL_SCANCODE_A}, {s, SDL_SCANCODE_S}, {d, SDL_SCANCODE_D},
    {f, SDL_SCANCODE_F}, {g, SDL_SCANCODE_G}, {h, SDL_SCANCODE_H},
    {j, SDL_SCANCODE_J}, {k, SDL_SCANCODE_K}, {l, SDL_SCANCODE_L},
    {z, SDL_SCANCODE_Z}, {x, SDL_SCANCODE_X}, {c, SDL_SCANCODE_C},
    {v, SDL_SCANCODE_V}, {b, SDL_SCANCODE_B}, {n, SDL_SCANCODE_N},
    {m, SDL_SCANCODE_M}, {space, SDL_SCANCODE_SPACE},
    {lctrl, SDL_SCANCODE_LCTRL}, {lalt, SDL_SCANCODE_LALT},
    {rctrl, SDL_SCANCODE_RCTRL}, {ralt, SDL_SCANCODE_RALT},
    {one, SDL_SCANCODE_1}, {two, SDL_SCANCODE_2}, {three, SDL_SCANCODE_3},
    {four, SDL_SCANCODE_4}, {five, SDL_SCANCODE_5}, {six, SDL_SCANCODE_6},
    {seven, SDL_SCANCODE_7}, {eight, SDL_SCANCODE_8}, {nine, SDL_SCANCODE_9},
    {zero, SDL_SCANCODE_0}, {f1, SDL_SCANCODE_F1}, {f2, SDL_SCANCODE_F2},
    {f3, SDL_SCANCODE_F3}, {f4, SDL_SCANCODE_F4}, {f5, SDL_SCANCODE_F5},
    {f6, SDL_SCANCODE_F6}, {f7, SDL_SCANCODE_F7}, {f8, SDL_SCANCODE_F8},
    {f9, SDL_SCANCODE_F9}, {f10, SDL_SCANCODE_F10}, {f11, SDL_SCANCODE_F11},
    {f12, SDL_SCANCODE_F12}, {tab, SDL_SCANCODE_TAB}, {up, SDL_SCANCODE_UP},
    {down, SDL_SCANCODE_DOWN}, {left, SDL_SCANCODE_LEFT}, {right, SDL_SCANCODE_RIGHT},
    {lshift, SDL_SCANCODE_LSHIFT}, {rshift, SDL_SCANCODE_RSHIFT}, {enter, SDL_SCANCODE_RETURN},
    {backspace, SDL_SCANCODE_BACKSPACE}, {slash, SDL_SCANCODE_SLASH},
    {backslash, SDL_SCANCODE_BACKSLASH}, {comma, SDL_SCANCODE_COMMA},
    {period, SDL_SCANCODE_PERIOD}, {minus, SDL_SCANCODE_MINUS}, {equals, SDL_SCANCODE_EQUALS},
    {esc, SDL_SCANCODE_ESCAPE},
};
SDL_Scancode string_to_scancode(std::string_view string) {
    return mappings.at(string);
}
