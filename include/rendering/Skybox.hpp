#pragma once

#include <Shader.hpp>
#include <Camera.hpp>
#include <glad/glad.h>

class Skybox {
public:
    Skybox(const char* faceFilePaths[], Shader* skyboxShader);
    ~Skybox();

    void draw(Camera& camera);

private:
    GLuint vao, vbo;
    GLuint textureId;
    Shader* shader;

    // helper for loading face textures to cubemap
    GLuint loadCubeMap(const char* faceFilePaths[]);
};