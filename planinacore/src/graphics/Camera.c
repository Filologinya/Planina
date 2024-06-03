#include "Camera.h"

#include "Logger.h"
#include "cglm/mat4.h"

static void camera_update_vectors(Camera* camera) {
  vec4 front_vec = {0, 0, -1, 0};
  vec4 out_front_vec;
  glm_mat4_mulv(camera->rotation, front_vec, out_front_vec);
  glm_vec4_copy3(out_front_vec, camera->front);
  vec4 right_vec = {1, 0, 0, 0};
  vec4 out_right_vec;
  glm_mat4_mulv(camera->rotation, right_vec, out_right_vec);
  glm_vec4_copy3(out_right_vec, camera->right);
  vec4 up_vec = {0, 1, 0, 0};
  vec4 out_up_vec;
  glm_mat4_mulv(camera->rotation, up_vec, out_up_vec);
  glm_vec4_copy3(out_up_vec, camera->up);
}

Camera* init_camera(vec3 position, f32 fov) {
  Camera* camera = (Camera*)malloc(sizeof(Camera));

  mat4 arr = GLM_MAT4_IDENTITY_INIT;
  glm_mat4_copy(arr, camera->rotation);
  glm_vec3_copy(position, camera->position);
  camera->fov = fov;

  camera_update_vectors(camera);

  return camera;
}
void free_camera(Camera* camera) {
  plog_debug("Free camera [%p]", camera);
  free(camera);
}

void camera_get_projection(Camera* camera, Window* window, mat4 out) {
  f32 aspect = (f32)window->width / (f32)window->height;
  glm_perspective(camera->fov, aspect, 0.1f, 100.0f, out);
}
void camera_get_view(Camera* camera, mat4 out) {
  vec3 front;
  glm_vec3_add(camera->position, camera->front, front);
  glm_lookat(camera->position, front, camera->up, out);
}

void camera_rotate(Camera* camera, f32 x, f32 y, f32 z) {
  vec3 vec_z = {0,0,1};
  glm_rotate(camera->rotation, z, vec_z);
  vec3 vec_y = {0,1,0};
  glm_rotate(camera->rotation, y, vec_y);
  vec3 vec_x = {1,0,0};
  glm_rotate(camera->rotation, x, vec_x);

  camera_update_vectors(camera);
}
