#pragma once
#include <glad/glad.h>
#include <Shader.hpp>
#include <Collision.hpp>
#include <Camera.hpp>

class DebugDrawer {
public:

    static void init();
    static void cleanup();

    // draw collision boxes
    static void drawAABB(Camera& camera, const AABB& box, glm::vec3 color = glm::vec3(0, 1, 0));

    // draw 3d origin gizmo (blender, minecraft)
    static void draw3DGizmo(Camera& camera);

    static void drawPoint(Camera& camera, const glm::vec3& point, const glm::vec3& color, float size = 0.5f);
    
private:
    static GLuint vao, vbo;
    static Shader* AABBShader;
    static Shader* gizmoShader;
    static Shader* pointShader;
};