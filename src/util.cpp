#include "util.h"
#include <luacode.h>
#include <fstream>
#include <ranges>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
constexpr static lua_CompileOptions compile_opts{};
namespace fs = std::filesystem;
namespace util {
std::optional<std::string> read_file(const fs::path &path) {
    if (not fs::exists(path)) {
        return std::nullopt;
    }
    std::ifstream file_in{};
    file_in.open(path.c_str());
    if (not file_in.is_open()) {
        return std::nullopt;
    }
    std::string curr_line{};
    std::stringstream file_stream{};
    while (std::getline(file_in,curr_line)) {
        file_stream << curr_line << '\n';
    }
    return file_stream.str();
}
std::string compile_source(std::string_view source) {
    size_t outsize{};
    char* temp = luau_compile(source.data(), source.size(), const_cast<lua_CompileOptions*>(&compile_opts), &outsize);
    std::string bytecode{};
    bytecode.resize(outsize);
    for (size_t i{}; i < outsize; ++i) {
        bytecode.push_back(*temp);
    }
    free(temp);
    return bytecode;
}
}
#ifdef _WIN32
void attach_console() {
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$", "r", stdin);
    }
}
#endif//_WIN32
