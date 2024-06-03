#include "GraphicsEngine.h"

#include <glad/glad.h>
#include <stdlib.h>
// It should go next for glad capability
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "InputHandler.h"
#include "Logger.h"
#include "ResourceManager.h"
#include "graphics/Camera.h"
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

  vec3 camera_position = {0, 0, 1};
  // 1.6f radians ~= 91*
  // 1.9f radians ~= 108*
  // 2.4f radians ~= 137*
  engine->camera = init_camera(camera_position, 2.4f);
  if (engine->camera == NULL) {
    plog_error("%s", "Can't init Camera");
    free_resource_manager(engine->resource);
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

f32 vertices[] = {
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
  Camera* camera = engine->camera;

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

  // Matrix for camera rotation and shaders proceeding it
  mat4 model_matrix = {
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  };
  mat4 unary_matrix = {
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  };

  mat4 proj_view_matrix;
  mat4 camera_view_matrix;

  // TODO: remove play
  // vec3 scale_vec = { 0.5f, 0.5f, 0.5f };
  // vec3 rotate_vec = { 0.0f, 0.0f, 1.0f };
  // glm_scale(model_matrix, scale_vec);
  // glm_rotate(model_matrix, 0.5f, rotate_vec);

  // speed
  f32 speed = 1.0f;
  f32 sensitivity = 1.0f;

  // camera rotation
  f32 cam_x = 0.0f;
  f32 cam_y = 0.0f;
  const f32 degrees_89 = 1.57;

  // timer
  f32 last_time = glfwGetTime();
  f32 delta = 0.0f;
  f32 current_time = 0.0f;

  while (!window_should_close(win)) {
    // == timer tick ==
    current_time = glfwGetTime();
    delta = current_time - last_time;
    plog_trace("Delta time: %f", delta);
    last_time = current_time;

    // == check and proceed input ==
    if (jpressed(input, GLFW_KEY_ESCAPE)) {
      window_set_should_close(win, 1);
    }
    if (jpressed(input, GLFW_KEY_TAB)) {
      input_toggle_cursor(input, win);
    }
    if (pressed(input, GLFW_KEY_W)) {
      glm_vec3_mulsubs(camera->front, -delta * speed, camera->position);
    }
    if (pressed(input, GLFW_KEY_A)) {
      glm_vec3_mulsubs(camera->right, delta * speed, camera->position);
    }
    if (pressed(input, GLFW_KEY_S)) {
      glm_vec3_mulsubs(camera->front, delta * speed, camera->position);
    }
    if (pressed(input, GLFW_KEY_D)) {
      glm_vec3_mulsubs(camera->right, -delta * speed, camera->position);
    }
    if (pressed(input, GLFW_KEY_SPACE)) {
      camera->position[1] += delta * speed;
    }
    if (pressed(input, GLFW_KEY_LEFT_CONTROL)) {
      camera->position[1] -= delta * speed;
    }
    if (jclicked(input, GLFW_MOUSE_BUTTON_1)) {
      glClearColor(0.2, 0, 0, 1);
    }

    // camera rotation
    if (cursor_moving_camera(input)) {
      cam_y += -sensitivity * inputDeltaY(input) / (float)win->width;
      if (cam_y < -degrees_89) {
        cam_y = -degrees_89;
      }
      if (cam_y > degrees_89) {
        cam_y = degrees_89;
      }
      cam_x += -sensitivity * inputDeltaX(input) / (float)win->height;

      glm_mat4_copy(unary_matrix, camera->rotation);
      camera_rotate(camera, cam_y, cam_x, 0);
    }

    // == Draw ==
    glClear(GL_COLOR_BUFFER_BIT);

    // camera matrix
    camera_get_projection(camera, engine->window, proj_view_matrix);
    camera_get_view(camera, camera_view_matrix);
    glm_mat4_mul(proj_view_matrix, camera_view_matrix, proj_view_matrix);

    // shader work
    shader_use(s_shader_program);
    shader_uniform_matrix(s_shader_program, "model", model_matrix);
    shader_uniform_matrix(s_shader_program, "projview", proj_view_matrix);

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
