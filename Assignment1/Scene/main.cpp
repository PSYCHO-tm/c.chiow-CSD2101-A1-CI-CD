/**
 * main.cpp
 *
 * Entry point for Desktop (Windows/GLFW) and WebGL (Emscripten) builds.
 * Android uses NativeTemplate.cpp + JNI instead of this file.
 *
 * All rendering is delegated to Renderer::Instance() — this file has no
 * knowledge of Triangle, Square, or any other concrete Model.
 */

#include "Platform.h"
#include "Renderer.h"

// ==========================================================================
// WebGL / Emscripten
// ==========================================================================
#ifdef PLATFORM_EMSCRIPTEN

#ifdef USE_GLFW

static GLFWwindow* g_window = nullptr;

static void fbsize_cb(GLFWwindow* /*win*/, int w, int h)
{
    Renderer::Instance().resize(w, h);
}

static void main_loop()
{
    if (glfwWindowShouldClose(g_window)) {
        emscripten_cancel_main_loop();
        glfwDestroyWindow(g_window);
        glfwTerminate();
        return;
    }
    glfwPollEvents();
    Renderer::Instance().render();
    glfwSwapBuffers(g_window);
}

int main()
{
    LOGI("GLPI Framework Intro - WebGL / Emscripten (GLFW)");

    if (!glfwInit()) { LOGE("glfwInit failed"); return -1; }

    glfwWindowHint(GLFW_CLIENT_API,            GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    g_window = glfwCreateWindow(800, 600, "GLPI Framework Intro", nullptr, nullptr);
    if (!g_window) { LOGE("glfwCreateWindow failed"); glfwTerminate(); return -1; }

    glfwMakeContextCurrent(g_window);
    glfwSetFramebufferSizeCallback(g_window, fbsize_cb);

    Renderer::Instance().initializeRenderer();
    int w, h;
    glfwGetFramebufferSize(g_window, &w, &h);
    Renderer::Instance().resize(w, h);

    emscripten_set_main_loop(main_loop, 0, 1);
    return 0;
}

#else // !USE_GLFW — SDL2 backend

static SDL_Window*   g_window = nullptr;
static SDL_GLContext g_glctx  = nullptr;

static void main_loop()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT)
            emscripten_cancel_main_loop();
    }
    Renderer::Instance().render();
    SDL_GL_SwapWindow(g_window);
}

int main()
{
    LOGI("GLPI Framework Intro : WebGL / Emscripten (SDL2)");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOGE("SDL_Init failed: %s", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    g_window = SDL_CreateWindow(
        "GLPI Framework Intro",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!g_window) { LOGE("SDL_CreateWindow failed: %s", SDL_GetError()); return -1; }

    g_glctx = SDL_GL_CreateContext(g_window);
    if (!g_glctx) { LOGE("SDL_GL_CreateContext failed: %s", SDL_GetError()); return -1; }

    Renderer::Instance().initializeRenderer();
    Renderer::Instance().resize(800, 600);

    emscripten_set_main_loop(main_loop, 0, 1);
    return 0;
}

#endif // USE_GLFW

// ==========================================================================
// Desktop — Windows / GLFW
// ==========================================================================
#elif defined(PLATFORM_WINDOWS)

#include <iostream>

static void framebufferSizeCB(GLFWwindow* /*win*/, int w, int h)
{
    Renderer::Instance().resize(w, h);
}

int main()
{
    std::cout << "GLPI Framework Intro - Desktop (GLFW)\n";
    std::cout << "Watch the triangle and square render. Press ESC to quit.\n";

    if (!glfwInit()) { std::cerr << "glfwInit failed\n"; return -1; }

    glfwWindowHint(GLFW_CLIENT_API,            GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_ANY_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "GLPI Framework Intro", nullptr, nullptr);
    if (!window) {
        std::cerr << "glfwCreateWindow failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr << "glewInit failed\n"; return -1; }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCB);

    Renderer::Instance().initializeRenderer();
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    Renderer::Instance().resize(w, h);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        Renderer::Instance().render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

#endif // PLATFORM_WINDOWS
