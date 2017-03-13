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
    unsigned pixel_size;
};


// no kerning support for now
// to do: implement valve solution
// for better font rendering
class Font_loader
{
public:
    Font_loader();
    ~Font_loader();
    Font_loader(const Font_loader&) = delete;

    Font loadFont(const std::string& filename, unsigned size) const;

private:
    static bool isCurrent;

    FT_Library ftLib;
};

#endif // FONT_LOADER_HPP
