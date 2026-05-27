#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include <unordered_map>

// handles basic shader operations
class Shader {
public:

    /**
     * @brief constructs shader from files
     * @param vertPath full path to vertex shader file
     * @param fragPath full path to fragment shader file
     */
    Shader(const char* vertPath, const char* fragPath);
    ~Shader();

    /**
     * @brief bind shader as active program 
     */
    void bind();

    /**
     * @brief unbinds shader as active program
     */
    void unbind();
    
    /**
     * @brief set bool uniform on shader
     * @param name uniform identifier
     * @param value uniform boolean value
     */
    void setUniformBool1(const char* name, const bool value);

    /**
     * @brief set int uniform on shader
     * @param name uniform identifier
     * @param value uniform int value
     */
    void setUniformInt1(const char* name, int value);

    /**
     * @brief set float uniform on shader
     * @param name uniform identifier
     * @param value uniform float value
     */
    void setUniformFloat1(const char* name, float value);

    /**
     * @brief set 3 value vector uniform on shader
     * @param name uniform identifier
     * @param value uniform glm::vec3 value
     */
    void setUniformVec3(const char* name, const glm::vec3& value);

    /**
    * @brief set 4 value vector uniform on shader
    * @param name uniform identifier
    * @param value uniform glm::vec4 value
    */
    void setUniformVec4(const char* name, const glm::vec4& value);

    /**
     * @brief set 4x4 matrix uniform on shader
     * @param name uniform identifier
     * @param value uniform glm::mat4 value
     */
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