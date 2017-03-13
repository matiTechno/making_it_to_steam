#ifndef SHADER_HPP
#define SHADER_HPP

#include "base.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class ShaderPart: public GL_Base
{
public:
    ShaderPart(GLenum shader_type);

    GLuint get_id() const;
};

class Shader: public GL_Base
{
public:
    // id_name must be unique or empty
    // it is used for error log
    Shader(const std::string& vertex_d, const std::string& fragment_d, const std::string& geometry_d, bool is_from_file,
           const std::string& id_name = std::string());

    void bind() const;
    GLint getUniLocation(const std::string& uniform_name) const;

private:
    static GLuint bound_id;
    static std::vector<std::string> id_names;

    std::unordered_map<std::string, GLint> uniform_locations;
    std::string id_name;

    std::string load_source_from_file(const std::string& filename);
    void load_uniform_locations();
    ShaderPart compile_shader(const std::string& source, GLenum shader_type);
    bool isError(bool isProgram, GLuint id, GLenum flag, const std::string& error_msg);
};

#endif // SHADER_HPP
