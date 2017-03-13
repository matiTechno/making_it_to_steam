#include "font_loader.hpp"
#include <stdexcept>
#include <vector>
#include "../res_class.h"

class Wrp_face: public Res_class<FT_Face>
{
public:
    Wrp_face(FT_Face id):
        Res_class([](FT_Face id){FT_Done_Face(id);})
{
    this->id = id;
}
};

bool Font_loader::isCurrent = false;

Font_loader::Font_loader()
{
    assert(!isCurrent);
    isCurrent = true;

    FT_Error error = FT_Init_FreeType(&ftLib);
    if(error)
        throw std::runtime_error("FreeType initialization failed: error: " + std::to_string(error));
}

Font_loader::~Font_loader()
{
    FT_Done_FreeType(ftLib);
}

Font Font_loader::loadFont(const std::string& filename, unsigned size) const
{
    FT_Face face;
    FT_Error error = FT_New_Face(ftLib, filename.c_str(), 0, &face);

    if(error == FT_Err_Unknown_File_Format)
        throw std::runtime_error("FreeType: unsupported file format: " + filename);
    else if(error)
        throw std::runtime_error("FreeType: FT_New_Face error: " + filename);

    Wrp_face wrp_face(face);

    error = FT_Set_Pixel_Sizes(face, 0, size);
    if(error)
        throw std::runtime_error("FreeType: could not set pixel size: " + filename);

    FT_GlyphSlot slot = face->glyph;
    int line_space = static_cast<int>(face->size->metrics.height >> 6);

    unsigned max_height = 0, summ_x = 0;
    int max_bearing_y = 0;

    for(std::size_t i = 32; i < 127; ++i)
    {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER))
            throw std::runtime_error("FreeType: not all ASCII printable chars avaible: " + filename);

        summ_x += slot->bitmap.width + 1;
        if(slot->bitmap.rows > max_height)
            max_height = slot->bitmap.rows;
        if(slot->bitmap_top > max_bearing_y)
            max_bearing_y = slot->bitmap_top;
    }

    GLint unpack_alignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::vector<char> clear_color(max_height * summ_x, 0);
    Texture atlas(GL_R8, static_cast<int>(summ_x), static_cast<int>(max_height));
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, atlas.getSize().x, atlas.getSize().y, GL_RED,
                    GL_UNSIGNED_BYTE, clear_color.data());

    std::unordered_map<char, Char> chars;
    int x = 0;

    for(std::size_t i = 32; i < 127; ++i)
    {
        FT_Load_Char(face, i, FT_LOAD_RENDER);

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, static_cast<int>(slot->bitmap.width),
                        static_cast<int>(slot->bitmap.rows),
                        GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);

        chars.emplace(i, Char{glm::ivec4(x, 0, slot->bitmap.width, slot->bitmap.rows),
                              glm::ivec2(slot->bitmap_left, slot->bitmap_top),
                              static_cast<int>(slot->advance.x >> 6)});

        x += slot->bitmap.width + 1;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_alignment);

    return Font{std::move(atlas), std::move(chars), line_space, max_bearing_y, size};
}
