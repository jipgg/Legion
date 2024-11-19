#include <blaze/Blaze.h>
#include <memory>
#include <SDL.h>
struct lua_State;
namespace builtin {

void register_matrix3_type(lua_State* L);
void register_vector2_type(lua_State* L);
void register_vector3_type(lua_State* L);
void register_vector_type(lua_State* L);
void register_path_type(lua_State* L);
void register_event_type(lua_State* L);
void register_font_type(lua_State* L);
void register_texture_type(lua_State* L);
void register_color_type(lua_State* L);

struct Rect {double x, y, w, h;};
using Color = SDL_Color;
using Vec2 = blaze::StaticVector<double, 2, blaze::defaultTransposeFlag, blaze::aligned, blaze::unpadded>;
using Vec3 = blaze::StaticVector<double, 3, blaze::defaultTransposeFlag, blaze::aligned,
    blaze::unpadded /*not 100% sure why but this causes issues when turning ud back to blaze type if set padded*/>; 
using Vec = blaze::DynamicVector<double>;
using Mat3 = blaze::StaticMatrix<double, 3, 3, blaze::defaultStorageOrder, blaze::aligned, blaze::unpadded>;
using FilePath = std::filesystem::path;
using DirectoryEntry = std::filesystem::directory_entry;
struct Event {
    struct Connection {
        size_t id;
    };
    std::vector<std::pair<size_t, int>> refs;
    lua_State* L;
    static constexpr size_t nullid = 0;
    size_t curr_id{nullid};
    Event(lua_State* L);
    ~Event();
    Connection connect(int idx);
    void disconnect(Connection id);
    void fire(int arg_count);
};
struct Texture {
    std::shared_ptr<SDL_Texture> ptr;
    int w, h;
};
struct Font {
    std::shared_ptr<TTF_Font> ptr;
    int pt_size;
    FilePath file_path;
};
}
