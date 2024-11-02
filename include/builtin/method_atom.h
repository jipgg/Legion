#pragma once
namespace builtin {
enum class method_atom {
    /*Vec2*/ dot, unit, abs, magnitude,
    /*fs_path*/ stem, empty, filename,
    hasStem, rootPath, parentPath, isRelative,
    isAbsolute, hasExtension, extension, replaceExtension,
    relativePath, hasRelativePath, compare, rootName,
    rootDirectory, hasRootPath, hasRootName, hasRootDirectory,
    /*fs_directory_entry*/ isDirectory, isFifo, path, isSocket,
    isOther, isRegularFile, isCharacterFile, isSymlink,
    isBlockFile,
    /*texture*/ size,
    /*M3x3*/ transpose, inverse, 
    /*Rect*/ bounds /*last one always*/
};
}
