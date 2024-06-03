#pragma once

#include "Types.h"
#include "Mesh.h"
#include "voxel/Chunk.h"

typedef struct {
  f32* buffer;
} VoxelRenderer;

VoxelRenderer* init_voxel_renderer(u32 capacity);
void free_voxel_renderer(VoxelRenderer* renderer);

Mesh* render(VoxelRenderer* renderer, Chunk* chunk);
