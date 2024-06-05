#include "Chunk.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "Logger.h"

void voxels_generation(Voxel* voxels, i32 xpos, i32 ypos, i32 zpos) {
  for (u32 y = 0; y < CHUNK_H; ++y) {
    for (u32 z = 0; z < CHUNK_D; ++z) {
      for (u32 x = 0; x < CHUNK_W; ++x) {
        i32 real_x = x + xpos * CHUNK_W;
        i32 real_y = y + ypos * CHUNK_H;
        i32 real_z = z + zpos * CHUNK_D;
 
        u8 id = 0;
        if (real_y <= 3) {
          id = 1;
        }
        //if (real_y <= ((sin(real_x * 0.6f)) * 0.5f + 0.5f) * 10) {
        //  id = 1;
        //}
        //if (real_y <= ((sin(x* 0.6f)) * 0.5f + 0.5f) * 5) {
        //  id = 2;
        //}
        voxels[(y * CHUNK_D + z) * CHUNK_W + x].id = id;
      }
    }
  }
}

Chunk* init_chunk(i32 x, i32 y, i32 z) {
  Chunk* chunk = (Chunk*)malloc(sizeof(Chunk));
  chunk->voxels = (Voxel*)malloc(sizeof(Voxel) * CHUNK_SIZE);
  memset(chunk->voxels, 0, sizeof(Voxel) * CHUNK_SIZE);
  chunk->x = x;
  chunk->y = y;
  chunk->z = z;
  plog_debug("Init chunk [%p] with voxels [%p] and position {%d, %d, %d}", chunk, chunk->voxels, chunk->x, chunk->y, chunk->z);

  voxels_generation(chunk->voxels, x, y, z);

  return chunk;
}
void free_chunk(Chunk* chunk) {
  plog_debug("Free chunk [%p] with voxels [%p]", chunk, chunk->voxels);

  free(chunk->voxels);
  free(chunk);
}
