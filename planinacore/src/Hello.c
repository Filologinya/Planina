#include "Hello.h"

#include <glad/glad.h>
// It should go next for glad capability
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "Logger.h"

int windowSizeX = 640;
int windowSizeY = 480;

void glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
  windowSizeX = width;
  windowSizeY = height;
  glViewport(0, 0, windowSizeX, windowSizeY);
}

Planina* init_planina() {
  Planina* planina = (Planina*)malloc(sizeof(Planina));

  plog_debug("Init planina: %p", planina);
  return planina;
}

void free_planina(Planina* planina) {
  plog_debug("Free planina: %p", planina);
  free(planina);
}

void planina_say_hello(Planina* planina) {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) {
    plog_error("%s", "glfwInit failed!");
    return;
  }
  plog_trace("Initialized glfw");

  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(windowSizeX, windowSizeY, "Planina", NULL, NULL);
  if (!window) {
    plog_error("%s", "Can't create window!");
    glfwTerminate();
    return;
  }
  plog_trace("Created window");

  glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  if (!gladLoadGL()) {
    plog_error("%s", "Can't load GLAD!");
    return;
  }

  plog_info("OpenGL %d.%d", GLVersion.major, GLVersion.minor);

  glClearColor(0, 1, 0, 1);

  plog_trace("Draw window");
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return;
}
