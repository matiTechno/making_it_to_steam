#ifndef FONT_LOADER_HPP
#define FONT_LOADER_HPP

#include <string>
#include "../opengl/texture.hpp"
#include <unordered_map>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
typedef struct FT_LibraryRec_  *FT_Library;

struct Char
{
    glm::ivec4 texCoords;
    glm::ivec2 bearing;
    int advance;
};

struct Font
{
    Texture atlas;
    std::unordered_map<char, Char> chars;
    int line_space;
    int max_bearing_y;
    int pixel_size;
};

// to do:
// * kerning ? (freetype support for kerning is not so good)
// * signed distance field rendering
// * better packing
// ...
// only ASCII glyphs
// size is in pixels
class Font_loader
{
public:
    Font_loader(int max_tex_size = 1024);
    ~Font_loader();
    Font_loader(const Font_loader&) = delete;

    Font loadFont(const std::string& filename, int size) const;

private:
    static bool isCurrent;

    FT_Library ftLib;
    const int max_tex_size;
};

#endif // FONT_LOADER_HPP
