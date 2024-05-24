#include "GraphicsEngine.h"

#include <glad/glad.h>
#include <stdlib.h>
// It should go next for glad capability
#include <GLFW/glfw3.h>

#include "InputHandler.h"
#include "Logger.h"
#include "graphics/Window.h"

GraphicsEngine* init_graphics_engine() {
  GraphicsEngine* engine = (GraphicsEngine*)malloc(sizeof(GraphicsEngine));
  plog_debug("Create GraphicsEngine [%p]", engine);

  engine->window = init_window(600, 400, "planina");
  if (engine->window == NULL) {
    plog_error("Can't init window");
    free(engine);
    return NULL;
  }

  engine->input = init_input_handler(engine->window);
  if (engine->input == NULL) {
    plog_error("%s", "Can't init InputHandler");
    free(engine->window);
    free(engine);
    return NULL;
  }

  return engine;
}
void free_graphics_engine(GraphicsEngine* engine) {
  plog_debug("Free GraphicsEngine [%p]", engine);
  free_window(engine->window);

  free(engine);
}

void engine_run(GraphicsEngine* engine) {
  Window* win = engine->window;
  InputHandler* input = engine->input;

  glClearColor(0, 0, 0, 1);
  while (!window_should_close(win)) {
    pull_events(input);

    if (jpressed(input, GLFW_KEY_ESCAPE)) {
      window_set_should_close(win, 1);
    }

    if (jclicked(input, GLFW_MOUSE_BUTTON_1)) {
      glClearColor(0.2, 0, 0, 1);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    window_swap_buffers(engine->window);
  }
}
