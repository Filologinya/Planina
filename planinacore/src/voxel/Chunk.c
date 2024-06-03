#include "Chunk.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Logger.h"

void voxels_generation(Voxel* voxels) {
  for (u32 y = 0; y < CHUNK_H; ++y) {
    for (u32 z = 0; z < CHUNK_D; ++z) {
      for (u32 x = 0; x < CHUNK_W; ++x) {
        u8 id = 0;
        if (y <= ((sin(x * 0.6f)) * 0.5f + 0.5f) * 10) {
          id = 1;
        }
        if (y <= ((sin(x * 0.6f)) * 0.5f + 0.5f) * 5) {
          id = 2;
        }
        voxels[(y * CHUNK_D + z) * CHUNK_W + x].id = id;
      }
    }
  }
}

Chunk* init_chunk() {
  Chunk* chunk = (Chunk*)malloc(sizeof(Chunk));
  chunk->voxels = (Voxel*)malloc(sizeof(Voxel) * CHUNK_SIZE);
  memset(chunk->voxels, 0, sizeof(Voxel) * CHUNK_SIZE);
  plog_debug("Init chunk [%p] with voxels [%p]", chunk, chunk->voxels);
  voxels_generation(chunk->voxels);

  return chunk;
}
void free_chunk(Chunk* chunk) {
  plog_debug("Free chunk [%p] with voxels [%p]", chunk, chunk->voxels);

  free(chunk->voxels);
  free(chunk);
}
