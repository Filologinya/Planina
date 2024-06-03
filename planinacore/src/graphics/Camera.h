#pragma once

#include <cglm/cglm.h>
#include "graphics/Window.h"
#include "Types.h"

typedef struct {
  mat4 rotation;
  vec3 front;
  vec3 up;
  vec3 right;

  vec3 position;
  f32 fov;
} Camera;

Camera* init_camera(vec3 position, f32 fov);
void free_camera(Camera* camera);

void camera_get_projection(Camera* camera, Window* window, mat4 out);
void camera_get_view(Camera* camera, mat4 out);

void camera_rotate(Camera* camera, f32 x, f32 y, f32 z);

