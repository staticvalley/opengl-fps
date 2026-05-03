#pragma once

#include <Skybox.hpp>
#include <Shader.hpp>
#include <Camera.hpp>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include "../libs/stb/stb_image.h"

// define unit cube for skybox
static const GLfloat SKYBOX_VERTICES[] = {
    -1, 1,-1,  -1,-1,-1,   1,-1,-1,   1,-1,-1,   1, 1,-1,  -1, 1,-1,
    -1,-1, 1,  -1,-1,-1,  -1, 1,-1,  -1, 1,-1,  -1, 1, 1,  -1,-1, 1,
    1,-1,-1,   1,-1, 1,   1, 1, 1,   1, 1, 1,   1, 1,-1,   1,-1,-1,
    -1,-1, 1,  -1, 1, 1,   1, 1, 1,   1, 1, 1,   1,-1, 1,  -1,-1, 1,
    -1, 1,-1,   1, 1,-1,   1, 1, 1,   1, 1, 1,  -1, 1, 1,  -1, 1,-1,
    -1,-1,-1,  -1,-1, 1,   1,-1, 1,   1,-1, 1,   1,-1,-1,  -1,-1,-1
};

Skybox::Skybox(const char* faceFilePaths[], Shader* skyboxShader) 
    : shader(skyboxShader)
{
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOX_VERTICES), SKYBOX_VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    textureId = loadCubeMap(faceFilePaths);
}

GLuint Skybox::loadCubeMap(const char* faceFilePaths[]) {

    // create and bind texture object as cube map
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int width, height, nChannels;
    for (int i = 0; i < 6; i++) {
        unsigned char* data = stbi_load(faceFilePaths[i], &width, &height, &nChannels, 0);
        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
        }
        else {
            SDL_Log("error loading skybox texture (face %d)", i);
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return id;
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &textureId);
    delete shader;
}

void Skybox::draw(Camera& camera) {
    
    // bind skybox specific shader
    shader->bind();

    // clip translation from view matrix so skybox is always at origin
    glm::mat4 view = glm::mat4(
        glm::mat3(camera.getViewMatrix())
    );

    shader->setUniformMat4("u_view", view);
    shader->setUniformMat4("u_projection", camera.getProjectionMatrix());

    // skybox texture at slot 0
    shader->setUniformInt1("u_skybox", 0);
    
    // draw box with different depth drawing function
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}