/*!
@file         glhelper.cpp
@author       pghali@digipen.edu
@co-author    parminder.singh@digipen.edu
@co-author    c.chiow@digipen.edu
@date         31/03/2024
@modified     30/06/2026

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and use GLEW to extract function
pointers to OpenGL implementations.

Changes for Assignment 1:
- mousebutton_cb: when in painter mode (GLPbo::is_painter_mode), implements
  freehand stroke start/stop. PRESS records the cursor as painter_start
  and painter_prev and begins the stroke (GLPbo::painter_dragging = true);
  RELEASE simply ends it. GLFW gives cursor positions with (0,0) at the
  top-left corner of the client area, so ypos is flipped to match OpenGL's
  bottom-left origin.
- mousepos_cb: while a stroke is active, commits a short Bresenham segment
  from GLPbo::painter_prev to the new cursor position directly into
  GLPbo::painter_canvas on every move, then advances painter_prev. This
  chains many short segments into a continuous freehand curve matching
  the sample executable's painter behavior.

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glhelper.h>
#include <glpbo.h>
#include <iostream>
#include <hook_csd2101.h>

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
GLint        GLHelper::width;
GLint        GLHelper::height;
GLdouble     GLHelper::fps;
GLdouble     GLHelper::delta_time;
std::string  GLHelper::title;
GLFWwindow* GLHelper::ptr_window;

/*  _________________________________________________________________________ */
/*! init
@param GLint width
@param GLint height
Dimensions of window requested by program

@param std::string title
String printed to window's title bar

@return bool
true if OpenGL context and GLEW were successfully initialized.
false otherwise.
*/
bool GLHelper::init(GLint width, GLint height, std::string title) {
    GLHelper::width = width;
    GLHelper::height = height;
    GLHelper::title = title;

    if (!glfwInit()) {
        std::cout << "GLFW init has failed - abort program!!!" << std::endl;
        return false;
    }

    glfwSetErrorCallback(GLHelper::error_cb);

    constexpr int REQUIRED_GL_MAJOR = 4;
    constexpr int REQUIRED_GL_MINOR = 5;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, REQUIRED_GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, REQUIRED_GL_MINOR);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLHelper::ptr_window = glfwCreateWindow(width, height, title.c_str(),
        nullptr, nullptr);
    if (!GLHelper::ptr_window) {
        std::cerr << "GLFW unable to create OpenGL context - abort program\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(GLHelper::ptr_window);
    setup_event_callbacks();
    glfwSetInputMode(GLHelper::ptr_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Unable to initialize GLEW - error: "
            << glewGetErrorString(err) << " abort program" << std::endl;
        return false;
    }
    if (GLEW_VERSION_4_5) {
        std::cout << "Using glew version: " << glewGetString(GLEW_VERSION) << std::endl;
        std::cout << "Driver supports OpenGL 4.5\n" << std::endl;
    }
    else {
        const GLubyte* glVersionStr = glGetString(GL_VERSION);
        if (glVersionStr) {
            int major = 0, minor = 0;
            if (sscanf(reinterpret_cast<const char*>(glVersionStr),
                "%d.%d", &major, &minor) == 2) {
                if (major > REQUIRED_GL_MAJOR ||
                    (major == REQUIRED_GL_MAJOR && minor >= REQUIRED_GL_MINOR)) {
                    std::cout << "Driver supports OpenGL "
                        << major << "." << minor << "\n" << std::endl;
                }
                else {
                    std::cerr << "Warning: OpenGL " << major << "." << minor
                        << " does not meet required "
                        << REQUIRED_GL_MAJOR << "." << REQUIRED_GL_MINOR
                        << std::endl;
                }
            }
        }
    }

    return true;
}

/*  _________________________________________________________________________ */
/*! setup_event_callbacks
@return none
Registers all GLFW event callbacks for the current window.
*/
void GLHelper::setup_event_callbacks() {
    AUTOMATION_HOOK_EVENTS();

    glfwSetFramebufferSizeCallback(GLHelper::ptr_window, GLHelper::fbsize_cb);
    glfwSetKeyCallback(GLHelper::ptr_window, GLHelper::key_cb);
    glfwSetMouseButtonCallback(GLHelper::ptr_window, GLHelper::mousebutton_cb);
    glfwSetCursorPosCallback(GLHelper::ptr_window, GLHelper::mousepos_cb);
    glfwSetScrollCallback(GLHelper::ptr_window, GLHelper::mousescroll_cb);
}

/*  _________________________________________________________________________ */
/*! cleanup
@return none
Terminates GLFW and returns all resources to the system.
*/
void GLHelper::cleanup() {
    glfwTerminate();
}

/*  _________________________________________________________________________ */
/*! key_cb
@param pwin     Handle to window receiving the event
@param key      GLFW key token
@param scancode Platform scancode
@param action   GLFW_PRESS, GLFW_REPEAT, or GLFW_RELEASE
@param mod      Modifier key bitmask
@return none

Closes the window on ESC.
*/
void GLHelper::key_cb(GLFWwindow* pwin, int key, int scancode,
    int action, int mod) {
    (void)scancode; (void)mod;

    if (GLFW_PRESS == action) {
#ifdef _DEBUG
        std::cout << "Key pressed" << std::endl;
#endif
    }
    else if (GLFW_REPEAT == action) {
#ifdef _DEBUG
        std::cout << "Key repeatedly pressed" << std::endl;
#endif
    }
    else if (GLFW_RELEASE == action) {
#ifdef _DEBUG
        std::cout << "Key released" << std::endl;
#endif
    }

    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
        glfwSetWindowShouldClose(pwin, GLFW_TRUE);
    }
}

/*  _________________________________________________________________________ */
/*! mousebutton_cb
@param pwin   Handle to window receiving the event
@param button Mouse button identifier (GLFW_MOUSE_BUTTON_LEFT, etc.)
@param action GLFW_PRESS or GLFW_RELEASE
@param mod    Modifier key bitmask
@return none

In painter mode, implements freehand drawing to match the sample
executable:
  - PRESS   : records the cursor position as both painter_start and
              painter_prev, and begins the stroke (painter_dragging =
              true). No segment is drawn yet - a single point alone has
              nothing to connect to.
  - RELEASE : simply ends the stroke. All visible drawing already
              happened incrementally in mousepos_cb as the mouse moved,
              so there is nothing left to commit here.

GLFW reports ypos measured from the top of the window; we flip it to
convert to OpenGL's bottom-left origin: y_gl = (window_height - 1) - y_glfw.

@co-author  c.chiow@digipen.edu
@modified   30/06/2026
Replaced click-drag-release-with-a-single-straight-segment with true
freehand drawing: the actual line segments are now committed continuously
in mousepos_cb as the cursor moves, so this function only needs to
start/stop the stroke.
*/
void GLHelper::mousebutton_cb(GLFWwindow* pwin, int button, int action, int mod) {
    (void)mod;

#ifdef _DEBUG
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        std::cout << "Left mouse button "
        << (action == GLFW_PRESS ? "pressed" : "released")
        << std::endl;
#endif

    if (button != GLFW_MOUSE_BUTTON_LEFT || !GLPbo::is_painter_mode)
        return;

    double xpos, ypos;
    glfwGetCursorPos(pwin, &xpos, &ypos);
    int x = static_cast<int>(xpos);
    int y = (GLHelper::height - 1) - static_cast<int>(ypos);

    if (action == GLFW_PRESS)
    {
        GLPbo::painter_start = glm::ivec2(x, y);
        GLPbo::painter_prev = glm::ivec2(x, y);
        GLPbo::painter_dragging = true;
    }
    else if (action == GLFW_RELEASE)
    {
        GLPbo::painter_dragging = false;
    }
}

/*  _________________________________________________________________________ */
/*! mousepos_cb
@param pwin  Handle to window receiving the event
@param xpos  Cursor x position (from left edge of client area)
@param ypos  Cursor y position (from top  edge of client area)
@return none

While a stroke is in progress (GLPbo::painter_dragging == true), commits a
short Bresenham segment from the last processed point (GLPbo::painter_prev)
to the new cursor position directly into GLPbo::painter_canvas, then
advances painter_prev to that new position. Chaining these short segments
together across many move events produces a continuous freehand curve
that follows wherever the cursor actually travels, matching the sample
executable's painter behavior.

The commit is done by temporarily redirecting GLPbo::ptr_to_pbo at the
canvas's own memory before calling render_linebresenham. This is safe
inside a GLFW callback because the real PBO is never mapped here -
GLPbo::emulate() only maps it once per frame, after glfwPollEvents() (and
therefore after all queued callbacks) have already run.

@co-author  c.chiow@digipen.edu
@modified   30/06/2026
Replaced the rubber-band-preview version with direct incremental
commits, switching the painter from "click sets a single straight
segment" to true freehand drawing.
*/
void GLHelper::mousepos_cb(GLFWwindow* pwin, double xpos, double ypos) {
    (void)pwin;

#ifdef _DEBUG
    std::cout << "Mouse cursor position: (" << xpos << ", " << ypos << ")"
        << std::endl;
#endif

    if (!GLPbo::is_painter_mode || !GLPbo::painter_dragging)
        return;

    int x = static_cast<int>(xpos);
    int y = (GLHelper::height - 1) - static_cast<int>(ypos);
    glm::ivec2 cur(x, y);

    if (cur == GLPbo::painter_prev || GLPbo::painter_canvas.empty())
        return; // nothing moved, or canvas not initialized yet

    GLPbo::Color* saved_ptr = GLPbo::ptr_to_pbo;
    GLPbo::ptr_to_pbo = GLPbo::painter_canvas.data();

    GLPbo::render_linebresenham(
        GLPbo::painter_prev.x, GLPbo::painter_prev.y,
        cur.x, cur.y,
        GLPbo::Color(255, 0, 0, 255)); // red

    GLPbo::ptr_to_pbo = saved_ptr;
    GLPbo::painter_prev = cur;
    GLPbo::painter_end = cur; // kept in sync for title-bar/debug use
}

/*  _________________________________________________________________________ */
/*! mousescroll_cb
@param pwin    Handle to window receiving the event
@param xoffset Horizontal scroll offset
@param yoffset Vertical   scroll offset
@return none
*/
void GLHelper::mousescroll_cb(GLFWwindow* pwin,
    double xoffset, double yoffset) {
    (void)pwin;
#ifdef _DEBUG
    std::cout << "Mouse scroll wheel offset: ("
        << xoffset << ", " << yoffset << ")" << std::endl;
#endif
}

/*  _________________________________________________________________________ */
/*! error_cb
@param error       GLFW error code
@param description Human-readable description
@return none
*/
void GLHelper::error_cb(int error, char const* description) {
    (void)error;
#ifdef _DEBUG
    std::cerr << "GLFW error: " << description << std::endl;
#endif
}

/*  _________________________________________________________________________ */
/*! fbsize_cb
@param ptr_win Handle to window being resized
@param width   New framebuffer width  in pixels
@param height  New framebuffer height in pixels
@return none
*/
void GLHelper::fbsize_cb(GLFWwindow* ptr_win, int width, int height) {
    (void)ptr_win;
#ifdef _DEBUG
    std::cout << "fbsize_cb getting called!!!" << std::endl;
#endif
    GLHelper::width = width;
    GLHelper::height = height;
}

/*  _________________________________________________________________________ */
/*! update_time
@param fps_calc_interval Interval in seconds at which FPS is recalculated
@return none

Must be called once per game loop.  Computes delta_time and fps.
*/
void GLHelper::update_time(double fps_calc_interval) {
    static double prev_time = glfwGetTime();
    double curr_time = glfwGetTime();
    delta_time = curr_time - prev_time;
    prev_time = curr_time;

    static double count = 0.0;
    static double start_time = glfwGetTime();
    double elapsed_time = curr_time - start_time;
    ++count;

    fps_calc_interval = (fps_calc_interval < 0.0) ? 0.0 : fps_calc_interval;
    fps_calc_interval = (fps_calc_interval > 10.0) ? 10.0 : fps_calc_interval;

    if (elapsed_time > fps_calc_interval) {
        GLHelper::fps = count / elapsed_time;
        start_time = curr_time;
        count = 0.0;
    }
}

/*  _________________________________________________________________________ */
/*! print_specs
@return none
Placeholder - copy implementation from Tutorial 1 if required.
*/
void GLHelper::print_specs() {
    // Copy from Tutorial 1
}