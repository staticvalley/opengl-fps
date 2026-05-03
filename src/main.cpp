#define SDL_MAIN_USE_CALLBACKS 1

#define DEBUG 1

#include <glad/glad.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_main.h>

// debug library
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <Shader.hpp>
#include <Camera.hpp>
#include <Mesh.hpp>
#include <Model.hpp>
#include <Skybox.hpp>
#include <Lighting.hpp>

#define WINDOW_WIDTH 800.0f
#define WINDOW_HEIGHT 600.0f

SDL_Window* window = nullptr;
SDL_GLContext opengl_context = nullptr;
Camera* camera;
Shader* basic;
Model* gman;
Model* silenthill;
Skybox* skybox;
Shader* skyboxShader;
PointLight pl;
Model* unitsquare;
GLfloat lastTime = 0.0f;

bool hasFlashlightOn = true;

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

    // create imgui context and bind to sdl3 window
    ImGui::CreateContext();
    ImGui::StyleColorsLight();

    ImGui_ImplSDL3_InitForOpenGL(window, opengl_context);
    ImGui_ImplOpenGL3_Init("#version 460");



    const char* skyboxFilePaths[] = {
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/up.jpg",
        "assets/textures/skybox/down.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg",
    };

    skybox = new Skybox(skyboxFilePaths, new Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag"));

    gman = Model::loadOBJ("assets/models/gman.obj");
    gman->position = glm::vec3(-1.0f, 0.0f, 0.0f);
    gman->scale = glm::vec3(2.0f);
    gman->setTexture(new Texture("assets/textures/gman.png"));

    silenthill = Model::loadOBJ("assets/models/heathermason.obj");
    silenthill->position = glm::vec3(1.0f, 0.0f, 0.0f);
    silenthill->scale = glm::vec3(0.003f);
    silenthill->setTexture(new Texture("assets/textures/heathermason.png"));

    unitsquare = Model::loadOBJ("assets/models/unitsquare.obj");
    unitsquare->position = glm::vec3(1.1f, 0.0f, -2.0f);
    unitsquare->setTexture(new Texture("assets/textures/flat.jpg"));

    basic = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");

    camera = new Camera(
        glm::vec3(1.0, 1.0, 2.0),
        90.0f,
        WINDOW_WIDTH / WINDOW_HEIGHT
    );

    pl.position = glm::vec3(0, 0, 0);
    pl.ambient_color = glm::vec3(0.2);
    pl.diffuse_color = glm::vec3(0.9);

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

    //gman->rotation.y = gman->rotation.y += 1.0f;
    //silenthill->rotation.y = silenthill->rotation.y += 1.0f;

    basic->bind();

    // camera uniforms
    basic->setUniformMat4("u_view", camera->getViewMatrix());
    basic->setUniformMat4("u_projection", camera->getProjectionMatrix());
    basic->setUniformVec3("u_camera_position", camera->position);

    // light uniforms
    basic->setUniformInt1("u_spot_light_count", 0);
    basic->setUniformBool1("u_has_directional_light", false);
    
    pl.position = glm::vec3(0, sin(currentTime) + 0.5, -0.5);

    //pl.position = glm::vec3(sin(currentTime)+0.5, 0, cos(currentTime) + 0.5);

    if (hasFlashlightOn) {
        basic->setUniformInt1("u_point_light_count", 1);
        pl.apply(*basic, 0);
    }
    else {
        basic->setUniformInt1("u_point_light_count", 0);
    }

    gman->draw(*basic);
    silenthill->draw(*basic);
    unitsquare->draw(*basic);

    //skybox->draw(*camera);
    
#ifdef DEBUG
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();

    ImGui::Begin("Debug");
    ImGui::Text("camera pos [%.2lf %.2lf %.2lf]", camera->position.x, camera->position.y, camera->position.z);
    ImGui::Text("camera view [%.2lf %.2lf %.2lf]", camera->forward.x, camera->forward.y, camera->forward.z);
    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

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
    case SDL_EVENT_KEY_DOWN:
        if (event->key.scancode == SDL_SCANCODE_F) hasFlashlightOn = !hasFlashlightOn;
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
    delete gman;
    delete silenthill;
    delete skybox;
    SDL_GL_DestroyContext(opengl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}