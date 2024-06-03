#include "Window.h"

#include <glad/glad.h>
// It should go next for glad capability
#include <GLFW/glfw3.h>
#include <stdlib.h>

#include "Logger.h"

// 0 for not initialized, 1+ for initialized
static i8 glfw_inited = 0;
static i8 glad_inited = 0;

Window* init_window(i32 width, i32 height, const char* title) {
  Window* window = (Window*)malloc(sizeof(Window));
  plog_debug("Allocate window [%p]", window);
  window->width = width;
  window->height = height;

  if (!glfw_inited) {
    glfw_inited = 1;
    if (!glfwInit()) {
      plog_error("%s", "glfwInit failed!");
      goto init_window_error;
    }
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    plog_trace("Initialized glfw");
  }
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a windowed mode window and its OpenGL context
  window->gl_window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window->gl_window) {
    plog_error("%s", "Can't create glfw window!");
    glfwTerminate();
    goto init_window_error;
  }
  plog_trace("Created window");

  /* Make the window's context current */
  glfwMakeContextCurrent(window->gl_window);

  if (!glad_inited) {
    glad_inited = 1;
    if (!gladLoadGL()) {
      plog_error("%s", "Can't load GLAD!");
      glfwTerminate();
      goto init_window_error;
    }
    plog_trace("Initialized GLAD");
  }
  plog_info("OpenGL %d.%d", GLVersion.major, GLVersion.minor);

  glViewport(0, 0, width, height);

  plog_trace("Init window with size {%d:%d} and title \"%s\"", width, height,
             title);
  return window;

init_window_error:
  plog_trace("%s", "Goto: label init_window_error");
  free(window);
  return NULL;
}

void free_window(Window* window) {
  plog_debug("Free window [%p]", window);
  free(window);
}

i32 window_should_close(Window* window) {
  return glfwWindowShouldClose(window->gl_window);
}

void window_set_should_close(Window* window, i32 code) {
  glfwSetWindowShouldClose(window->gl_window, code);
}

void window_swap_buffers(Window* window) { glfwSwapBuffers(window->gl_window); }

void window_set_cursor_mode(Window* window, i32 mode) {
  glfwSetInputMode(window->gl_window, GLFW_CURSOR, mode);
}
