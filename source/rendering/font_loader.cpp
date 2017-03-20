#include "font_loader.hpp"
#include <stdexcept>
#include <vector>
#include "../res_class.hpp"
#include <algorithm>

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

Font_loader::Font_loader(int max_tex_size):
    max_tex_size(max_tex_size)
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

Font Font_loader::loadFont(const std::string& filename, int size) const
{
    FT_Face face;
    FT_Error error = FT_New_Face(ftLib, filename.c_str(), 0, &face);

    if(error == FT_Err_Unknown_File_Format)
        throw std::runtime_error("FreeType: unsupported file format: " + filename);
    else if(error)
        throw std::runtime_error("FreeType: FT_New_Face error: " + filename);

    Wrp_face wrp_face(face);
    (void)wrp_face;

    error = FT_Set_Pixel_Sizes(face, 0, static_cast<unsigned>(size));
    if(error)
        throw std::runtime_error("FreeType: could not set pixel size: " + filename);

    FT_GlyphSlot slot = face->glyph;
    int line_space = static_cast<int>(face->size->metrics.height >> 6);

    int max_bitmap_height = 0, max_bearing_y = 0;
    std::vector<int> x_sizes;
    x_sizes.emplace_back(0);

    for(std::size_t i = 32; i < 127; ++i)
    {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER))
            throw std::runtime_error("FreeType: not all ASCII printable chars avaible: " + filename);

        if(static_cast<int>(slot->bitmap.rows) > max_bitmap_height)
            max_bitmap_height = static_cast<int>(slot->bitmap.rows);
        if(slot->bitmap_top > max_bearing_y)
            max_bearing_y = slot->bitmap_top;

        int add_width = static_cast<int>(slot->bitmap.width) + 1;
        if(x_sizes.back() + add_width > max_tex_size)
            x_sizes.emplace_back();

        x_sizes.back() += add_width;
    }

    int tex_width = *std::max_element(x_sizes.begin(), x_sizes.end());
    int tex_height = (max_bitmap_height + 1) * static_cast<int>(x_sizes.size()) - 1;

    if(tex_height > max_tex_size)
        throw std::runtime_error("texture size = " + std::to_string(tex_height) + " needed to contain " + filename +
                                 " (pixel size = " + std::to_string(size) +
                                 ") exceeds Font_loader::max_tex_size = " + std::to_string(max_tex_size));

    // textCoords and texture
    GLint unpack_alignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::unordered_map<char, Char> chars;
    Texture atlas(GL_R8, tex_width, tex_height);
    std::vector<char> clear_color(static_cast<std::size_t>(atlas.getSize().x * atlas.getSize().y), 0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, atlas.getSize().x, atlas.getSize().y, GL_RED,
                    GL_UNSIGNED_BYTE, clear_color.data());

    glm::ivec2 pos(0, 0);

    for(std::size_t i = 32; i < 127; ++i)
    {
        FT_Load_Char(face, i, FT_LOAD_RENDER);

        if(pos.x + static_cast<int>(slot->bitmap.width) + 1 > tex_width)
        {
            pos.x = 0;
            pos.y += max_bitmap_height + 1;
        }

        glm::ivec4 texCoords(pos, static_cast<int>(slot->bitmap.width),
                             static_cast<int>(slot->bitmap.rows));

        glTexSubImage2D(GL_TEXTURE_2D, 0, texCoords.x, texCoords.y, texCoords.z, texCoords.w, GL_RED,
                        GL_UNSIGNED_BYTE, slot->bitmap.buffer);

        chars.emplace(i, Char{std::move(texCoords), glm::ivec2(slot->bitmap_left, slot->bitmap_top),
                              static_cast<int>(slot->advance.x >> 6)});

        pos.x += static_cast<int>(slot->bitmap.width) + 1;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_alignment);
    return Font{std::move(atlas), std::move(chars), line_space, max_bearing_y, size};
}
