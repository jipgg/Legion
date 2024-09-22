#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <filesystem>
namespace util {
[[nodiscard]] std::string read_text_file(const std::filesystem::path& path);
}/*util*/
#endif
