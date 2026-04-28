#define SDL_MAIN_USE_CALLBACKS 1

#include <glad/glad.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_main.h>

#include <../include/Shader.hpp>
#include <../include/Camera.hpp>
#include <../include/Mesh.hpp>
#include <../include/Model.hpp>

#define WINDOW_WIDTH 800.0f
#define WINDOW_HEIGHT 600.0f

SDL_Window* window = nullptr;
SDL_GLContext opengl_context = nullptr;
Camera* camera;
Shader* basic;
Model* model;
GLfloat lastTime = 0.0f;

// program initialization
SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {

    // initialize sdl video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("sdl3 video subsystem could not be initialized: %s",
            SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // set opengl version and use core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // create sdl window
    window = SDL_CreateWindow(
        "opengl_fps ver. 1", WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        SDL_Log("sdl3 window creation failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // create context and attach to sdl window
    opengl_context = SDL_GL_CreateContext(window);
    if (!opengl_context) {
        SDL_Log("sdl3 opengl context creation failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // load glad gl functions
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_Log("glad failed to initialize");
        return SDL_APP_FAILURE;
    }

    // init sdl event subsystem
    if (!SDL_Init(SDL_INIT_EVENTS)) {
        SDL_Log("sdl3 event subsystem could not be initialized: %s",
            SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // enable z testing for drawing
    glEnable(GL_DEPTH_TEST);

    // hide mouse when window is focused
    SDL_SetWindowRelativeMouseMode(window, true);

    std::vector<Vertex> vertices = {
    { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0,0,1), glm::vec2(0,0) },
    { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0,0,1), glm::vec2(1,0) },
    { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0,0,1), glm::vec2(1,1) },
    { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0,0,1), glm::vec2(0,1) },
    };

    std::vector<GLuint> indices = { 0, 1, 2,  2, 3, 0 };

    model = new Model();
    model->addMesh(Mesh(vertices, indices));
    model->position = glm::vec3(0.0f, 0.0f, 0.0f);

    basic = new Shader("assets/basic.vert", "assets/basic.frag");

    camera = new Camera(
        glm::vec3(0.0, 0.0, 0.0),
        90.0f,
        WINDOW_WIDTH / WINDOW_HEIGHT
    );

    return SDL_APP_CONTINUE;
}

// main program loop
SDL_AppResult SDL_AppIterate(void* appstate) {

    // calculate delta time
    float currentTime = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    camera->processKeyboard(deltaTime);

    // clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    basic->bind();
    basic->setUniformMat4("u_view", camera->getViewMatrix());
    basic->setUniformMat4("u_projection", camera->getProjectionMatrix());
    model->draw(*basic);
    
    // switch buffer to screen
    SDL_GL_SwapWindow(window);

    return SDL_APP_CONTINUE;
}

// event handler
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    switch (event->type) {
    case SDL_EVENT_MOUSE_MOTION:
        camera->processMouse(event->motion.xrel, event->motion.yrel);
        break;
    case SDL_EVENT_WINDOW_RESIZED:
        // set new aspect ratio for projection
        camera->aspectRatio = (float)event->window.data1 / (float)event->window.data2;
        glViewport(0, 0, event->window.data1, event->window.data2);
        break;
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

// no cleanup needed, sdl cleans up its own entities
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    delete camera;
    delete basic;
    delete model;
    SDL_GL_DestroyContext(opengl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}