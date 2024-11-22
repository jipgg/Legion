#pragma once
#include <cstdint>

struct Color3u8 {
    uint8_t r{0xff}, g{0xff}, b{0xff};
    enum ColorPreset {White, Black, Red, Green, Blue, Cyan, Magenta, Yellow};
    static consteval Color3u8 preset(ColorPreset c) {
        switch (c) {
            case White: return Color3u8{0xff, 0xff, 0xff};
            case Black: return Color3u8{0, 0, 0};
            case Red: return Color3u8{0xff, 0, 0};
            case Green: return Color3u8{0, 0xff, 0};
            case Blue: return Color3u8{0, 0, 0xff};
            case Cyan: return Color3u8{0, 0xff, 0xff};
            case Magenta: return Color3u8{0xff, 0, 0xff};
            case Yellow: return Color3u8{0xff, 0xff, 0};
        }
    }
};
struct Color4u8 {
    uint8_t r{0xff}, g{0xff}, b{0xff}, a{0xff};
};
