#pragma once

#include <cglm/cglm.h>
#include <stdlib.h>

#include "Chunk.h"
#include "Types.h"

typedef struct {
  Chunk** chunks;
  u32 volume;
  i32 w; 
  i32 h; 
  i32 d;
} Chunks;

Chunks* init_chunks(i32 w, i32 h, i32 d);
void free_chunks(Chunks* chunks);

Chunk* chunks_get_chunk(Chunks* c, i32 x, i32 y, i32 z);
Voxel* chunks_get(Chunks* c, i32 x, i32 y, i32 z);
void chunks_set(Chunks* c, i32 id, i32 x, i32 y, i32 z);
Voxel* chunks_ray_cast(Chunks* c, vec3 start, vec3 dir, float maxLength, vec3* end,
                       vec3* norm, vec3* iend);

