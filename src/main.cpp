#include <cassert>
#include "legion.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
int main(int, char**) {
    const engine::Start_options opts {
        .window_name{"ENGINE"},
        .window_size{800, 600},
        .window_resizable = true,
        .hardware_accelerated = true,
        .vsync_enabled = true,
    };
    return engine::bootstrap(opts);
}
