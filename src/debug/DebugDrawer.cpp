// DebugDraw.cpp
#include <DebugDrawer.hpp>
#include <Camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SDL3/SDL.h>

#define DEBUG_BUFFER_SIZE 512

GLuint DebugDrawer::vao = 0;
GLuint DebugDrawer::vbo = 0;

Shader* DebugDrawer::AABBShader = nullptr;
Shader* DebugDrawer::gizmoShader = nullptr;
Shader* DebugDrawer::pointShader = nullptr;

void DebugDrawer::init() {

    AABBShader = new Shader("assets/shaders/debug/debugAABB.vert", "assets/shaders/debug/debugAABB.frag");
    gizmoShader = new Shader("assets/shaders/debug/debug3DGizmo.vert", "assets/shaders/debug/debug3DGizmo.frag");
    pointShader = AABBShader;

    // create vertex buffer and array
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // bind array and buffer to add data and attributes
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // set an empty buffer for all draw ops
    glBufferData(GL_ARRAY_BUFFER, DEBUG_BUFFER_SIZE * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    
    // add attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    
    glBindVertexArray(0);
}

void DebugDrawer::drawAABB(Camera& camera, const AABB& box, glm::vec3 color) {

    glm::vec3 min = box.min;
    glm::vec3 max = box.max;

    glm::vec3 boxLines[] = {
        // bottom square
        {min.x,min.y,min.z}, {max.x,min.y,min.z},
        {max.x,min.y,min.z}, {max.x,min.y,max.z},
        {max.x,min.y,max.z}, {min.x,min.y,max.z},
        {min.x,min.y,max.z}, {min.x,min.y,min.z},
        // top square
        {min.x,max.y,min.z}, {max.x,max.y,min.z},
        {max.x,max.y,min.z}, {max.x,max.y,max.z},
        {max.x,max.y,max.z}, {min.x,max.y,max.z},
        {min.x,max.y,max.z}, {min.x,max.y,min.z},
        // vertical edges
        {min.x,min.y,min.z}, {min.x,max.y,min.z},
        {max.x,min.y,min.z}, {max.x,max.y,min.z},
        {max.x,min.y,max.z}, {max.x,max.y,max.z},
        {min.x,min.y,max.z}, {min.x,max.y,max.z},
    };

    // insert box data to buffer
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(boxLines), boxLines);

    // draw
    AABBShader->bind();
    AABBShader->setUniformMat4("u_view", camera.getViewMatrix());
    AABBShader->setUniformMat4("u_projection", camera.getProjectionMatrix());
    AABBShader->setUniformVec3("u_color", color);
    glDrawArrays(GL_LINES, 0, 24);

    // unbind vao
    glBindVertexArray(0);
}

void DebugDrawer::draw3DGizmo(Camera& camera) {
    
    // dont vanish behind geometry
    glDisable(GL_DEPTH_TEST);

    glm::vec3 axisLines[] = {
        {0,0,0}, {1,0,0},
        {0,0,0}, {0,1,0},
        {0,0,0}, {0,0,1}
    };

    // set the 3d gizmos position infront of the camera by a bit
    glm::vec3 gizmoPos = camera.getPosition() + camera.getForwardDirection() * 2.0f;
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), gizmoPos);

    // scale down gizmo a bit
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));

    // insert gizmo data
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(axisLines), axisLines);

    // draw
    gizmoShader->bind();
    gizmoShader->setUniformMat4("u_view", camera.getViewMatrix());
    gizmoShader->setUniformMat4("u_projection", camera.getProjectionMatrix());
    gizmoShader->setUniformMat4("u_model", modelMatrix);
    glDrawArrays(GL_LINES, 0, 6);

    // unbind vao and re-enable z testing
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
}

void DebugDrawer::drawPoint(Camera& camera, const glm::vec3& point, const glm::vec3& color, float size) {
    
    glm::vec3 lines[] = {
        point - glm::vec3(size, 0, 0), point + glm::vec3(size, 0, 0),
        point - glm::vec3(0, size, 0), point + glm::vec3(0, size, 0),
        point - glm::vec3(0, 0, size), point + glm::vec3(0, 0, size),
    };

    // dont vanish behind geometry
    glDisable(GL_DEPTH_TEST);

    // insert line data
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lines), lines);

    // draw
    pointShader->bind();
    pointShader->setUniformMat4("u_view", camera.getViewMatrix());
    pointShader->setUniformMat4("u_projection", camera.getProjectionMatrix());
    pointShader->setUniformMat4("u_model", glm::mat4(1.0f));
    pointShader->setUniformVec3("u_color", color);
    glDrawArrays(GL_LINES, 0, 6);
    
    // unbind vao and re-enable z testing
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
}

void DebugDrawer::cleanup() {
    delete AABBShader;
    delete gizmoShader;
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

