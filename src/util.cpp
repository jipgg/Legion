#include "util.h"
#include <fstream>
#include <cassert>
#include <sstream>
namespace fs = std::filesystem;
namespace util {
std::string read_text_file(const fs::path &path) {
    assert(fs::exists(path));
    std::ifstream file_in{};
    file_in.open(path.c_str());
    assert(file_in.is_open());
    std::string curr_line{};
    std::stringstream file_stream{};
    while (std::getline(file_in,curr_line)) {
        file_stream << curr_line << '\n';
    }
    return file_stream.str();
}
}
