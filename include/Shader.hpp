#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include <unordered_map>

// handles basic shader operations
class Shader {
public:

    // compiles and links shader programs
    Shader(const char* vertPath, const char* fragPath);
    ~Shader();

    // bind shader for use in draw calls
    void bind();

    // unbind shader
    void unbind();

    // ###############
    // uniform setters
    // ###############

    // set integer uniform on shader
    void setUniformInt1(const char* name, int value);

    // set float uniform on shader
    void setUniformFloat1(const char* name, float value);

    // set vec4 uniform on shader
    void setUniformVec4(const char* name, const glm::vec4& value);

    // set vec3 uniform on shader
    void setUniformVec3(const char* name, const glm::vec3& value);

    // set 4x4 matrix uniform on shader
    void setUniformMat4(const char* name, const glm::mat4& value);

private:
    // shader id
    GLuint id;

    // shader uniform name cache
    std::unordered_map<std::string, GLint> uniformCache;
    
    // check shader for uniform name location
    GLint getUniformLocation(const char* name);

    // parse shader file to string
    std::string parseShaderFile(const char* filePath);

    // compile vert/frag shader given source code
    GLuint compileShader(GLuint type, const std::string& source);
};