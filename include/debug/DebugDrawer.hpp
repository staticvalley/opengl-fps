#pragma once
#include <glad/glad.h>
#include <Shader.hpp>
#include <Collision.hpp>
#include <Camera.hpp>

/**
 * @brief static functions for debugging visually in 3D space
 */
class DebugDrawer {
public:
    /**
     * @brief set up and initialize draw buffer and vao
     */
    static void init();

    /**
     * @brief deallocate draw buffer and vao
     */
    static void cleanup();

    /**
     * @brief draws axis-aligned bounding box wireframe
     * @param camera active scene camera for view/projection matrices
     * @param box axis-aligned bounding box
     * @param color rendered wifeframe color
     */
    static void drawAABB(Camera& camera, const AABB& box, glm::vec3 color = glm::vec3(0, 1, 0));

    /**
     * @brief draws 3D axis orientation gizmo
     * @param camera active scene camera for view/projection matrices
     */
    static void draw3DGizmo(Camera& camera);

    /**
     * @brief draws axis-aligned star in world space at point
     * @param camera active scene camera for view/projection matrices
     * @param point origin of rendered point in world space
     * @param color rendered point color
     * @param size point size scaler
     */
    static void drawPoint(Camera& camera, const glm::vec3& point, const glm::vec3& color, float size = 0.5f);
    
private:

    static GLuint vao, vbo;
    static Shader* AABBShader;
    static Shader* gizmoShader;
    static Shader* pointShader;
};