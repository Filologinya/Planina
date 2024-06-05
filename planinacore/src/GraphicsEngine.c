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
#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/VoxelRenderer.h"
#include "graphics/Window.h"
#include "voxel/Chunks.h"

// World and rendering
static Chunks* chunks;
static Mesh** meshes;
static VoxelRenderer* renderer;

// shaders
static Shader* s_shader_program;
static Shader* s_crosshair_shader;
static Mesh* s_crosshair_mesh;
// for crosshair shader
float vertices[] = {
    // x    y
    -0.01f, -0.01f, 0.01f, 0.01f,

    -0.01f, 0.01f,  0.01f, -0.01f,
};

int attrs[] = {
    2, 0  // null terminator
};

GraphicsEngine* init_graphics_engine(int argc, char* argv[]) {
  GraphicsEngine* engine = (GraphicsEngine*)malloc(sizeof(GraphicsEngine));
  plog_debug("Create GraphicsEngine [%p]", engine);
  if (engine == NULL) {
    return NULL;
  }

  engine->window = init_window(1200, 800, "planina");
  if (engine->window == NULL) {
    plog_error("Can't init window");
    goto engine_init_err;
  }

  engine->input = init_input_handler(engine->window);
  if (engine->input == NULL) {
    plog_error("%s", "Can't init InputHandler");
    goto engine_init_err;
  }

  engine->resource = init_resource_manager(argv[0], "res", "non-exist");
  if (engine->resource == NULL) {
    plog_error("%s", "Can't init InputHandler");
    goto engine_init_err;
  }

  vec3 camera_position = {64, 10, 64};
  // 1.6f radians ~= 91*
  // 1.9f radians ~= 108*
  // 2.4f radians ~= 137*
  engine->camera = init_camera(camera_position, 2.4f);
  if (engine->camera == NULL) {
    plog_error("%s", "Can't init Camera");
    goto engine_init_err;
  }

  s_shader_program = init_shader(engine->resource, "main.glslv", "main.glslf");
  if (s_shader_program == NULL) {
    plog_error("%s", "Can't load voxels shaders");
    goto engine_init_err;
  }

  s_crosshair_shader = init_shader(engine->resource, "crosshair.glslv", "crosshair.glslf");
  if (s_crosshair_shader == NULL) {
    plog_error("Can't load crosshair shader");
    goto engine_init_err;
  }
  s_crosshair_mesh = init_mesh(vertices, 4, attrs);


  // world and rendering
  chunks = init_chunks(8, 1, 8);
  if (chunks == NULL) {
    plog_error("Can't init chunks");
    goto engine_init_err;
  }
  meshes = (Mesh**)malloc(sizeof(Mesh) * chunks->volume);
  if (meshes == NULL) {
    plog_error("Can't init voxels meshes");
    goto engine_init_err;
  }
  for (u32 i = 0; i < chunks->volume; ++i) {
    meshes[i] = NULL;
  }
  renderer = init_voxel_renderer(1024 * 1024 * 8);
  if (renderer == NULL) {
    plog_error("Can't init renderer");
    goto engine_init_err;
  }
  // Init meshes
  Chunk* closes[27];
  for (size_t i = 0; i < chunks->volume; i++) {
    Chunk* chunk = chunks->chunks[i];
    chunk->modified = 0;

    for (int i = 0; i < 27; i++) closes[i] = NULL;
    for (size_t j = 0; j < chunks->volume; j++) {
      Chunk* other = chunks->chunks[j];

      int ox = other->x - chunk->x;
      int oy = other->y - chunk->y;
      int oz = other->z - chunk->z;

      if (abs(ox) > 1 || abs(oy) > 1 || abs(oz) > 1) continue;

      ox += 1;
      oy += 1;
      oz += 1;
      closes[(oy * 3 + oz) * 3 + ox] = other;
    }
    Mesh* mesh = render(renderer, chunk, (const Chunk**)closes, 1);
    meshes[i] = mesh;
  }

  return engine;


engine_init_err:
  if (chunks)
    free_chunks(chunks);
  if (renderer)
    free_voxel_renderer(renderer);
  if (meshes)
    free(meshes);
  if (s_shader_program)
    free_shader(s_shader_program);
  if (s_crosshair_shader)
    free_shader(s_crosshair_shader);
  if (engine->resource)
    free_resource_manager(engine->resource);
  if (engine->input)
    free_input_handler(engine->input);
  if (engine->window)
    free_window(engine->window);
  if (engine)
    free(engine);

  return NULL;
}
void free_graphics_engine(GraphicsEngine* engine) {
  plog_debug("Free GraphicsEngine [%p]", engine);
  free_window(engine->window);
  free_shader(s_shader_program);

  free(engine);
}


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


  glClearColor(0.6f, 0.62f, 0.65f, 1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Matrix for camera rotation and shaders proceeding it
  mat4 unary_matrix = {
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  };
  mat4 model_matrix = {
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  };

  mat4 proj_view_matrix;
  mat4 camera_view_matrix;

  // block type
  u8 block_type = 1;

  // speed
  f32 speed = 7.0f;
  f32 sensitivity = 0.7f;

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
    // plog_trace("Delta time: %f", delta);
    last_time = current_time;

    // == check and proceed input ==
    // Change block type
    if (jpressed(input, GLFW_KEY_1)) {
      block_type = 1;
    }
    if (jpressed(input, GLFW_KEY_2)) {
      block_type = 2;
    }

    // Save world
    if (jpressed(input, GLFW_KEY_3)) {
      u32 size = chunks->volume * CHUNK_SIZE;
      u8* buffer = (u8*)malloc(size);
      chunks_write(chunks, buffer);

      write_res_file(engine->resource, "world.bin", (const char*)buffer, size);
      free(buffer);
    }
    if (jpressed(input, GLFW_KEY_4)) {
      u8* buffer = (u8*)read_res_file(engine->resource, "world.bin");
      chunks_read(chunks, buffer);
      free(buffer);
    }
    // Close window
    if (jpressed(input, GLFW_KEY_ESCAPE)) {
      window_set_should_close(win, 1);
    }
    // Movement
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

    // Put or delete block
    {
      vec3 end;
      vec3 norm;
      vec3 iend;
      Voxel* vox = chunks_ray_cast(chunks, camera->position, camera->front,
                                   10.0f, &end, &norm, &iend);
      if (vox != NULL) {
        if (jclicked(input, GLFW_MOUSE_BUTTON_1)) {
          plog_trace("break block {%f, %f, %f}", (float)iend[0], (float)iend[1],
                     (float)iend[2]);
          chunks_set(chunks, 0, (int)iend[0], (int)iend[1], (int)iend[2]);
        }
        if (jclicked(input, GLFW_MOUSE_BUTTON_2)) {
          plog_trace("put block {%d, %d, %d}", (int)(iend[0]) + (int)(norm[0]),
                     (int)(iend[1]) + (int)(norm[1]),
                     (int)(iend[2]) + (int)(norm[2]));
          chunks_set(chunks, block_type, (int)(iend[0]) + (int)(norm[0]),
                     (int)(iend[1]) + (int)(norm[1]),
                     (int)(iend[2]) + (int)(norm[2]));
        }
      }
    }

    // Processing nearest chunks for right vision
    Chunk* closes[27];
    for (size_t i = 0; i < chunks->volume; i++) {
      Chunk* chunk = chunks->chunks[i];
      if (!chunk->modified) continue;
      plog_trace("Chunk (%d) modified", i);
      chunk->modified = 0;
      if (meshes[i] != NULL) {
        free_mesh(meshes[i]);
      }

      for (int i = 0; i < 27; i++) closes[i] = NULL;
      for (size_t j = 0; j < chunks->volume; j++) {
        Chunk* other = chunks->chunks[j];

        int ox = other->x - chunk->x;
        int oy = other->y - chunk->y;
        int oz = other->z - chunk->z;

        if (abs(ox) > 1 || abs(oy) > 1 || abs(oz) > 1) continue;

        ox += 1;
        oy += 1;
        oz += 1;
        closes[(oy * 3 + oz) * 3 + ox] = other;
      }
      Mesh* mesh = render(renderer, chunk, (const Chunk**)closes, 1);
      meshes[i] = mesh;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // == Draw chunks ==
    // camera matrix
    camera_get_projection(camera, engine->window, proj_view_matrix);
    camera_get_view(camera, camera_view_matrix);
    glm_mat4_mul(proj_view_matrix, camera_view_matrix, proj_view_matrix);
    // shader work
    shader_use(s_shader_program);
    shader_uniform_matrix(s_shader_program, "projview", proj_view_matrix);
    texture_bind(texture);

    for (u32 i = 0; i < chunks->volume; i++) {
      Chunk* chunk = chunks->chunks[i];
      Mesh* mesh = meshes[i];
      if (mesh == NULL) continue;

      f32 real_x = chunk->x * CHUNK_W + 0.5f;
      f32 real_y = chunk->y * CHUNK_H + 0.5f;
      f32 real_z = chunk->z * CHUNK_D + 0.5f;

      vec3 translate_vec = {real_x, real_y, real_z};
      glm_translate_to(unary_matrix, translate_vec, model_matrix);
      shader_uniform_matrix(s_shader_program, "model", model_matrix);
      mesh_draw(mesh, GL_TRIANGLES);
    }

    // == Crosshair draw ==
    shader_use(s_crosshair_shader);
    mesh_draw(s_crosshair_mesh, GL_LINES);

    window_swap_buffers(engine->window);
    pull_events(input);
  }

  free_voxel_renderer(renderer);
  free_texture(texture);
  for (u32 i = 0; i < chunks->volume; ++i) {
    if (meshes[i] != NULL) free_mesh(meshes[i]);
  }
}
