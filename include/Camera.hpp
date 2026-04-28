#pragma once

#define SEND_U_PROJECTION 1
#define SEND_U_VIEW 2
#define SEND_U_CAMERA_POSITION 4

#include <glm/glm.hpp>
#include <glad/glad.h>

class Camera {
public:

    Camera(glm::vec3 startPosition, GLfloat fov, GLfloat aspectRatio);
    ~Camera();
   
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    void processKeyboard(GLfloat dt);
    void processMouse(GLfloat x, GLfloat y);

    // changable by window resize
    GLfloat aspectRatio;

private:
    // current camera position
    glm::vec3 position;

    // camera relative front
    glm::vec3 forward;

    // camera relative right
    glm::vec3 right;

    // camera relative up
    glm::vec3 up;

    // field of view
    GLfloat fov;
    
    // right/left view
    GLfloat yaw;

    // up/down view
    GLfloat pitch;
    
    GLfloat mouseSensitivity;
    GLfloat movementSpeed;

    // calculate camera relative front and up vectors
    void calcuateRelativeVectors();
};