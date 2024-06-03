#pragma once

#include "Types.h"

typedef struct {
  u32 vertices;
  u32 vao;
  u32 vbo;
} Mesh;

Mesh* init_mesh(const f32* buffer, u32 vertices, const i32* attrs);
void free_mesh(Mesh* mesh);

void mesh_draw(Mesh* mesh, u32 primitive);
