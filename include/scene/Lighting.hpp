#pragma once

#include <Shader.hpp>

#include <glm/glm.hpp>
#include <string>

struct DirectionalLight {
	glm::vec3 direction = glm::vec3(0.0f);
	glm::vec3 ambient_color = glm::vec3(0.05f);
	glm::vec3 diffuse_color = glm::vec3(0.8f);
	glm::vec3 specular_color = glm::vec3(1.0f);

    void apply(Shader& shader) {
        shader.setUniformVec3("u_directional_light.direction", direction);
        shader.setUniformVec3("u_directional_light.ambient_color", ambient_color);
        shader.setUniformVec3("u_directional_light.diffuse_color", diffuse_color);
        shader.setUniformVec3("u_directional_light.specular_color", specular_color);
    }
};

struct PointLight {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 ambient_color = glm::vec3(0.05f);
    glm::vec3 diffuse_color = glm::vec3(0.8f);
    glm::vec3 specular_color = glm::vec3(1.0f);

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    void apply(Shader& shader, int index) {
        std::string baseString = "u_point_lights[" + std::to_string(index) + "].";
        shader.setUniformVec3((baseString + "position").c_str(), position);
        shader.setUniformVec3((baseString + "ambient_color").c_str(), ambient_color);
        shader.setUniformVec3((baseString + "diffuse_color").c_str(), diffuse_color);
        shader.setUniformVec3((baseString + "specular_color").c_str(), specular_color);
        shader.setUniformFloat1((baseString + "constant").c_str(), constant);
        shader.setUniformFloat1((baseString + "linear").c_str(), linear);
        shader.setUniformFloat1((baseString + "quadratic").c_str(), quadratic);
    }
};

struct SpotLight {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 ambient_color = glm::vec3(0.0f);
    glm::vec3 diffuse_color = glm::vec3(1.0f);
    glm::vec3 specular_color = glm::vec3(1.0f);

    float cutoff = glm::cos(glm::radians(12.5f));
    float outer_cutoff = glm::cos(glm::radians(17.5f));
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    void apply(Shader& shader, int index) {
        std::string baseString = "u_spot_lights[" + std::to_string(index) + "].";
        shader.setUniformVec3((baseString + "position").c_str(), position);
        shader.setUniformVec3((baseString + "direction").c_str(), direction);
        shader.setUniformVec3((baseString + "ambient_color").c_str(), ambient_color);
        shader.setUniformVec3((baseString + "diffuse_color").c_str(), diffuse_color);
        shader.setUniformVec3((baseString + "specular_color").c_str(), specular_color);
        shader.setUniformFloat1((baseString + "cutoff").c_str(), cutoff);
        shader.setUniformFloat1((baseString + "outer_cutoff").c_str(), outer_cutoff);
        shader.setUniformFloat1((baseString + "constant").c_str(), constant);
        shader.setUniformFloat1((baseString + "linear").c_str(), linear);
        shader.setUniformFloat1((baseString + "quadratic").c_str(), quadratic);
    }
};