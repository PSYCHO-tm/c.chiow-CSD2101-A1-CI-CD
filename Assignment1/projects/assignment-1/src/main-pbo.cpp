/*!
@file       main-pbo.cpp
@author     pghali@digipen.edu
@co-author  parminder.singh@digipen.edu
@date       28/06/2024

This file uses functionality defined in types GLHelper and GLPbo to 
initialize an OpenGL context and implement a game loop.

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glhelper.h>
#include <glpbo.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <hook_csd2101.h>
// Don't include glapp.h - we've nothing more to do with that file anymore!!!

/*                                                         type declarations
----------------------------------------------------------------------------- */

/*                                                      function declarations
----------------------------------------------------------------------------- */
static void draw();
static void update();
static void init(GLint width, GLint height, std::string title);
static void cleanup();

/*                                                      function definitions
----------------------------------------------------------------------------- */
/*  _________________________________________________________________________ */
/*! main

@param none

@return int

Indicates how the program existed. Normal exit is signaled by a return value of
0. Abnormal termination is signaled by a non-zero return value.
Note that the C++ compiler will insert a return 0 statement if one is missing.
*/
int main(int argc, char* argv[]) {
  ParseArguments& args = ParseArguments::getInstance();
  if (!args.parseArguments(argc, argv)) return 0;
    
  // Part 1
  init(1000, 1000, "A1");

  AUTOMATION_HOOK_RENDER(args); // Automation hook. [!WARNING!] Do not alter/remove this!

  // Part 2
  while (!glfwWindowShouldClose(GLHelper::ptr_window)) {
    // Part 2a
    update();
    // Part 2b
    draw();
  }

  // Part 3
  cleanup();
}

/*  _________________________________________________________________________ */
/*! update
@param none
@return none

Uses GLHelper::GLFWWindow* to get handle to OpenGL context.
For now, there are no objects to animate nor keyboard, mouse button click,
mouse movement, and mouse scroller events to be processed.
*/
static void update() {
  // Part 1
  glfwPollEvents();

  // Part 2
  GLHelper::update_time(1.0);

  // Part 3
  GLPbo::emulate();
}

/*  _________________________________________________________________________ */
/*! draw
@param none
@return none

Uses GLHelper::GLFWWindow* to get handle to OpenGL context.
For now, there's nothing to draw - just paint color buffer with constant color
*/
static void draw() {
  // Part 1
  GLPbo::draw_fullwindow_quad();

  // Part 2: swap buffers: front <-> back
  glfwSwapBuffers(GLHelper::ptr_window);
}

/*  _________________________________________________________________________ */
/*! init
@param none
@return none

Get handle to OpenGL context through GLHelper::GLFWwindow*.
*/
static void init(GLint width, GLint height, std::string title) {
  // Part 1
  if (!GLHelper::init(width, height, title)) {
    std::cout << "Unable to create OpenGL context" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Part 2
  GLPbo::init(GLHelper::width, GLHelper::height);
}

/*  _________________________________________________________________________ */
/*! cleanup
@param none
@return none

Return allocated resources for window and OpenGL context thro GLFW back
to system.
Return graphics memory claimed through 
*/
void cleanup() {
  // Part 1
  GLPbo::cleanup();

  // Part 2
  GLHelper::cleanup();
}
