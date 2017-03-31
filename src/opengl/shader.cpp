#include <MITS/opengl/shader.hpp>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <vector>

GLuint Shader::bound_id = 0;

ShaderPart::ShaderPart(GLenum shader_type):
    GL_Base([](GLuint id){glDeleteShader(id);})
{
    id = glCreateShader(shader_type);
}

GLuint ShaderPart::get_id() const
{
    return id;
}

Shader::Shader(const std::string& vertex_d, const std::string& fragment_d, const std::string& geometry_d, bool is_from_file,
               const std::string& id_name):
    GL_Base([](GLuint id){glDeleteProgram(id);}),
    id_name(id_name)
{
    std::unique_ptr<ShaderPart> vertex_shader, fragment_shader, geometry_shader;

    if(is_from_file)
    {
        vertex_shader = std::make_unique<ShaderPart>(compile_shader(load_source_from_file(vertex_d), GL_VERTEX_SHADER));
        fragment_shader = std::make_unique<ShaderPart>(compile_shader(load_source_from_file(fragment_d), GL_FRAGMENT_SHADER));
        if(geometry_d.size())
            geometry_shader = std::make_unique<ShaderPart>(compile_shader(load_source_from_file(geometry_d), GL_GEOMETRY_SHADER));
    }
    else
    {
        vertex_shader = std::make_unique<ShaderPart>(compile_shader(vertex_d, GL_VERTEX_SHADER));
        fragment_shader = std::make_unique<ShaderPart>(compile_shader(fragment_d, GL_FRAGMENT_SHADER));
        if(geometry_d.size())
            geometry_shader = std::make_unique<ShaderPart>(compile_shader(geometry_d, GL_GEOMETRY_SHADER));
    }

    bool vertex_error = false, fragment_error = false, geometry_error = false;

    std::string pre_msg = id_name + ": ";

    vertex_error = isError(false, vertex_shader->get_id(), GL_COMPILE_STATUS, pre_msg + "vertex shader compilation error:");
    fragment_error = isError(false, fragment_shader->get_id(), GL_COMPILE_STATUS, pre_msg + "fragment shader compilation error:");
    if(geometry_d.size())
        geometry_error = isError(false, geometry_shader->get_id(), GL_COMPILE_STATUS, pre_msg + "geometry shader compilation error:");

    if(vertex_error || fragment_error || geometry_error)
        throw std::exception();

    id = glCreateProgram();

    glAttachShader(id, vertex_shader->get_id());
    glAttachShader(id, fragment_shader->get_id());
    if(geometry_shader)
        glAttachShader(id, geometry_shader->get_id());
    glLinkProgram(id);

    if(isError(true, id, GL_LINK_STATUS, pre_msg + "program linking error:"))
    {
        glDeleteProgram(id);
        throw std::exception();
    }

    glDetachShader(id, vertex_shader->get_id());
    glDetachShader(id, fragment_shader->get_id());
    if(geometry_shader)
        glDetachShader(id, geometry_shader->get_id());

    load_uniform_locations();
}

void Shader::bind() const
{
    if(bound_id != id)
    {
        bound_id = id;
        glUseProgram(id);
    }
}

GLint Shader::getUniLocation(const std::string& uniform_name) const
{
    auto uniform_loc = uniform_locations.find(uniform_name);
    if(uniform_loc == uniform_locations.end())
        throw std::runtime_error(id_name + ": " + "no active uniform: " + uniform_name);
    return uniform_loc->second;
}

std::string Shader::load_source_from_file(const std::string& filename)
{
    std::ifstream ifs(filename);
    if(!ifs)
        throw std::runtime_error("could not open: " + filename);
    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

void Shader::load_uniform_locations()
{
    GLint num_uniforms;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &num_uniforms);
    std::vector<char> uniform_name(256, 0);

    for(GLuint i = 0; i < static_cast<GLuint>(num_uniforms); ++i)
    {
        GLint dummy1;
        GLenum dummy2;
        glGetActiveUniform(id, i, static_cast<GLsizei>(uniform_name.size()), nullptr, &dummy1, &dummy2,
                           uniform_name.data());

        GLint location = glGetUniformLocation(id, uniform_name.data());
        uniform_locations[uniform_name.data()] = location;
    }
}

ShaderPart Shader::compile_shader(const std::string& source, GLenum shader_type)
{
    ShaderPart shaderPart(shader_type);
    const GLchar* source_gl = source.c_str();
    glShaderSource(shaderPart.get_id(), 1, &source_gl, nullptr);
    glCompileShader(shaderPart.get_id());
    return shaderPart;
}

bool Shader::isError(bool isProgram, GLuint id, GLenum flag, const std::string& error_msg)
{
    GLint success;
    if(isProgram)
        glGetProgramiv(id, flag, &success);
    else
        glGetShaderiv(id, flag, &success);

    if(success == GL_FALSE)
    {
        GLint length;
        if(isProgram)
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
        else
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        std::vector<GLchar> infoLog(static_cast<std::size_t>(length));

        if(isProgram)
            glGetProgramInfoLog(id, length, nullptr, infoLog.data());
        else
            glGetShaderInfoLog(id, length, nullptr, infoLog.data());

        std::cout << error_msg << std::endl;
        std::cout << infoLog.data() << std::endl;

        return true;
    }
    return false;
}
