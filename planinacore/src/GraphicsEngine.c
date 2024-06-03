#include "GraphicsEngine.h"

#include <glad/glad.h>
#include <stdlib.h>
// It should go next for glad capability
#include <GLFW/glfw3.h>

#include "InputHandler.h"
#include "Logger.h"
#include "ResourceManager.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/Window.h"

static Shader* s_shader_program;

GraphicsEngine* init_graphics_engine(int argc, char* argv[]) {
  GraphicsEngine* engine = (GraphicsEngine*)malloc(sizeof(GraphicsEngine));
  plog_debug("Create GraphicsEngine [%p]", engine);

  engine->window = init_window(1200, 800, "planina");
  if (engine->window == NULL) {
    plog_error("Can't init window");
    free(engine);
    return NULL;
  }

  engine->input = init_input_handler(engine->window);
  if (engine->input == NULL) {
    plog_error("%s", "Can't init InputHandler");
    free_window(engine->window);
    free(engine);
    return NULL;
  }

  engine->resource = init_resource_manager(argv[0], "res", "non-exist");
  if (engine->resource == NULL) {
    plog_error("%s", "Can't init InputHandler");
    free_input_handler(engine->input);
    free_window(engine->window);
    free(engine);
    return NULL;
  }

  s_shader_program = init_shader(engine->resource, "main.glslv", "main.glslf");
  if (s_shader_program == NULL) {
    plog_error("%s", "Can't load shaders");
    free_resource_manager(engine->resource);
    free_input_handler(engine->input);
    free_window(engine->window);
    free(engine);
    return NULL;
  }

  return engine;
}
void free_graphics_engine(GraphicsEngine* engine) {
  plog_debug("Free GraphicsEngine [%p]", engine);
  free_window(engine->window);
  free_shader(s_shader_program);

  free(engine);
}

float vertices[] = {
    // x    y     z     u     v
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  //
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,   //
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,   //
                                     //
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,   //
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,    //
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,   //
};

void engine_run(GraphicsEngine* engine) {
  Window* win = engine->window;
  InputHandler* input = engine->input;

  // Texture
  glEnable(GL_TEXTURE_2D);
  Texture* texture = init_texture(engine->resource, "texture.png");
  if (texture == NULL) {
    plog_error("Can't load texture");
  }

  // Create vertices part
  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (GLvoid*)(0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  glClearColor(0.6f, 0.62f, 0.65f, 1);

  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  while (!window_should_close(win)) {
    if (jpressed(input, GLFW_KEY_ESCAPE)) {
      window_set_should_close(win, 1);
    }

    if (jclicked(input, GLFW_MOUSE_BUTTON_1)) {
      glClearColor(0.2, 0, 0, 1);
    }
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw VAO
    shader_use(s_shader_program);
    texture_bind(texture);

    glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

    window_swap_buffers(engine->window);
    pull_events(input);
  }

  free_texture(texture);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}
