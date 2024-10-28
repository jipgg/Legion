#pragma once
namespace builtin {
enum class method_atom {
    /*Vec2*/ dot, unit, abs, magnitude,
    /*fs_path*/ stem, empty, filename,
    has_stem, root_path, parent_path, is_relative,
    is_absolute, has_extension, extension, replace_extension,
    relative_path, has_relative_path, compare, root_name,
    root_directory, has_root_path, has_root_name, has_root_directory,
    /*fs_directory_entry*/ is_directory, is_fifo, path, is_socket,
    is_other, is_regular_file, is_character_file, is_symlink,
    is_block_file,
    /*texture*/ size,
    /*Rect*/ bounds /*last one always*/
};
}
