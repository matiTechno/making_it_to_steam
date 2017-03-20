#ifndef FONT_LOADER_HPP
#define FONT_LOADER_HPP

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include "../opengl/texture.hpp"
#include <unordered_map>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

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

// no kerning support for now
// to do: implement valve solution and better packing
// font is looking perfectly only when rendered pixel to pixel
// sometimes you can soften it with bloom if scaled or rotated
// (for example when implementing dynamic effects)
// only ASCII chars are loaded
// max_tex_size is temporary (proper packing needs to be implemented)
// if texture size needed to contain font bitmaps will be bigger
// than max_tex_size exception will be thrown
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
