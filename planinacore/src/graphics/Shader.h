#pragma once

#include "ResourceManager.h"
#include "Types.h"
#include <cglm/cglm.h>

typedef struct {
  u32 id;
} Shader;

Shader* init_shader(ResourceManager* res, const char* vertex_file,
                    const char* fragment_file);
void free_shader(Shader* shader);

void shader_use(Shader* shader);
void shader_uniform_matrix(Shader* shader, const char* name, mat4 matrix);
