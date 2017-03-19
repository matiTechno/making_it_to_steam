#include "text.hpp"
#include <vector>
#include <algorithm>
#include <assert.h>

void Text::set_pixel_size(unsigned size)
{
    scale = static_cast<float>(size) / static_cast<float>(font->pixel_size);
}

glm::vec2 Text::getSize() const
{
    std::size_t num_lines = static_cast<std::size_t>(std::count(text.begin(), text.end(), '\n')) + 1;
    std::size_t current_line = 0;
    std::vector<float> lines_width(num_lines, 0);
    float text_height = static_cast<float>(font->max_bearing_y) * scale;
    float max_below_origin = 0.f;

    for(auto c = text.begin(); c != text.end(); ++c)
    {
        if(*c == '\n')
        {
            ++current_line;
            text_height += static_cast<float>(font->line_space) * scale;
        }
        else
        {
            auto& this_char = font->chars.at(*c);

            if(*(c + 1) == '\n' || (c + 1) == text.end())
                lines_width[current_line] += static_cast<float>(this_char.bearing.x +
                                                                this_char.texCoords.z) * scale;
            else
                lines_width[current_line] += static_cast<float>(this_char.advance) * scale;

            if(current_line == num_lines - 1)
            {
                float below_origin = static_cast<float>(this_char.texCoords.w - this_char.bearing.y) * scale;

                if(below_origin > max_below_origin)
                    max_below_origin = below_origin;
            }
        }
    }
    auto max_width = std::max_element(lines_width.begin(), lines_width.end());
    return glm::vec2(*max_width, text_height + max_below_origin);
}
