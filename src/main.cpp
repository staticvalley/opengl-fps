#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_main.h>

#include <../include/Shader.hpp>
#include <../include/Camera.hpp>

#include <../include/Testbox.hpp>

#define WINDOW_WIDTH 800.0f
#define WINDOW_HEIGHT 600.0f

SDL_Window* window = nullptr;
SDL_GLContext opengl_context = nullptr;
Camera camera(
    glm::vec3(0.0, 0.0, 0.0),
    90.0f,
    WINDOW_WIDTH / WINDOW_HEIGHT
);
Shader basic("assets/basic.vert", "assets/basic.frag");
GLfloat lastTime = 0.0f;

// program initialization
SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("sdl3 video subsystem could not be initialized: %s",
            SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(
        "opengl_fps ver. 1", WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        SDL_Log("sdl3 window creation failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    opengl_context = SDL_GL_CreateContext(window);
    if (!opengl_context) {
        SDL_Log("sdl3 opengl context creation failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_Init(SDL_INIT_EVENTS)) {
        SDL_Log("sdl3 event subsystem could not be initialized: %s",
            SDL_GetError());
        return SDL_APP_FAILURE;
    }

    basic.bind();

    return SDL_APP_CONTINUE;
}

// main program loop
SDL_AppResult SDL_AppIterate(void* appstate) {

    // calculate delta time
    float currentTime = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    camera.processKeyboard(deltaTime);

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // switch buffer to screen
    SDL_GL_SwapWindow(window);

    return SDL_APP_CONTINUE;
}

// event handler
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    switch (event->type) {
    case SDL_EVENT_MOUSE_MOTION:
        camera.processMouse(event->motion.xrel, event->motion.yrel);
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
    SDL_GL_DestroyContext(opengl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}