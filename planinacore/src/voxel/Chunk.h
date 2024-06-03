#pragma once

#include "Voxel.h"

#define CHUNK_W 16
#define CHUNK_H 16
#define CHUNK_D 16
#define CHUNK_SIZE (CHUNK_W * CHUNK_H * CHUNK_D)

// Chunk
//
// Voxels iteration as:
// voxel = chunk[ (y * D + z) * W + x ] 
typedef struct {
  Voxel* voxels;
} Chunk;

Chunk* init_chunk();
void free_chunk(Chunk* chunk);

