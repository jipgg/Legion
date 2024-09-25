#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <filesystem>
#include <string_view>
#include <optional>
namespace util {/////UTIL/////
[[nodiscard]] std::optional<std::string> read_file(const std::filesystem::path& path);
[[nodiscard]] std::string compile_source(std::string_view string);
#ifdef _WIN32
void attach_console();
#endif
}/////UTIL/////END/////
#endif
