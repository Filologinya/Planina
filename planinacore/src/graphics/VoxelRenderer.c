#include "VoxelRenderer.h"

#include <stdlib.h>

#include "Logger.h"
#include "voxel/Chunk.h"
#include "voxel/Voxel.h"

const i32 VERTEX_SIZE = 3 + 2 + 1;

const int chunk_attrs[] = {3, 2, 1, 0};  // with null terminator

#define CDIV(X, A) (((X) < 0) ? ((X) / (A)-1) : ((X) / (A)))
#define LOCAL_NEG(X, SIZE) (((X) < 0) ? ((SIZE) + (X)) : (X))
#define LOCAL(X, SIZE) ((X) >= (SIZE) ? ((X) - (SIZE)) : LOCAL_NEG(X, SIZE))
#define IS_CHUNK(X, Y, Z) (GET_CHUNK(X, Y, Z) != NULL)
#define GET_CHUNK(X, Y, Z)                                          \
  (chunks[((CDIV(Y, CHUNK_H) + 1) * 3 + CDIV(Z, CHUNK_D) + 1) * 3 + \
          CDIV(X, CHUNK_W) + 1])

#define VOXEL(X, Y, Z)                                                        \
  (GET_CHUNK(X, Y, Z)                                                         \
       ->voxels[(LOCAL(Y, CHUNK_H) * CHUNK_D + LOCAL(Z, CHUNK_D)) * CHUNK_W + \
                LOCAL(X, CHUNK_W)])
#define IS_BLOCKED(X, Y, Z) ((!IS_CHUNK(X, Y, Z)) || VOXEL(X, Y, Z).id)

#define VERTEX(INDEX, X,Y,Z, U,V, L) buffer[INDEX+0] = (X);\
								  buffer[INDEX+1] = (Y);\
								  buffer[INDEX+2] = (Z);\
								  buffer[INDEX+3] = (U);\
								  buffer[INDEX+4] = (V);\
								  buffer[INDEX+5] = (L);\
								  INDEX += VERTEX_SIZE;

inline static void vertex(f32* buffer, u32* index, f32 x, f32 y, f32 z, f32 u,
                          f32 v, f32 l) {
  buffer[*index + 0] = x;
  buffer[*index + 1] = y;
  buffer[*index + 2] = z;
  buffer[*index + 3] = u;
  buffer[*index + 4] = v;
  buffer[*index + 5] = l;
  (*index) += VERTEX_SIZE;
}

VoxelRenderer* init_voxel_renderer(u32 capacity) {
  VoxelRenderer* renderer = (VoxelRenderer*)malloc(sizeof(VoxelRenderer));
  renderer->buffer = (f32*)malloc(sizeof(f32) * capacity * VERTEX_SIZE * 6);
  plog_debug("Init renderer [%p] with buffer [%p] size (%d)", renderer,
             renderer->buffer, capacity);

  return renderer;
}
void free_voxel_renderer(VoxelRenderer* renderer) {
  plog_debug("Init renderer [%p] with buffer [%p]", renderer, renderer->buffer);

  free(renderer->buffer);
  free(renderer);
}

Mesh* render(VoxelRenderer* renderer, Chunk* chunk, const Chunk** chunks,
             i8 ambientOcclusion) {
  f32* buffer = renderer->buffer;
  float aoFactor = 0.15f;
  u32 index = 0;
  for (int y = 0; y < CHUNK_H; y++) {
    for (int z = 0; z < CHUNK_D; z++) {
      for (int x = 0; x < CHUNK_W; x++) {
        Voxel vox = chunk->voxels[(y * CHUNK_D + z) * CHUNK_W + x];
        unsigned int id = vox.id;

        if (!id) {
          continue;
        }

        float l;
        float uvsize = 1.0f / 16.0f;
        float u1 = (id % 16) * uvsize;
        float v1 = 1 - ((1 + id / 16) * uvsize);

        // Ambient Occlusion values
        float a, b, c, d, e, f, g, h;
        a = b = c = d = e = f = g = h = 0.0f;

        float u2 = u1 + uvsize;
        float v2 = v1 + uvsize;

        if (!IS_BLOCKED(x, y + 1, z)) {
          l = 1.0f;

          if (ambientOcclusion) {
            a = IS_BLOCKED(x + 1, y + 1, z) * aoFactor;
            b = IS_BLOCKED(x, y + 1, z + 1) * aoFactor;
            c = IS_BLOCKED(x - 1, y + 1, z) * aoFactor;
            d = IS_BLOCKED(x, y + 1, z - 1) * aoFactor;

            e = IS_BLOCKED(x - 1, y + 1, z - 1) * aoFactor;
            f = IS_BLOCKED(x - 1, y + 1, z + 1) * aoFactor;
            g = IS_BLOCKED(x + 1, y + 1, z + 1) * aoFactor;
            h = IS_BLOCKED(x + 1, y + 1, z - 1) * aoFactor;
          }

          VERTEX(index, x - 0.5f, y + 0.5f, z - 0.5f, u2, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x - 0.5f, y + 0.5f, z + 0.5f, u2, v2,
                 l * (1.0f - c - b - f));
          VERTEX(index, x + 0.5f, y + 0.5f, z + 0.5f, u1, v2,
                 l * (1.0f - a - b - g));

          VERTEX(index, x - 0.5f, y + 0.5f, z - 0.5f, u2, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x + 0.5f, y + 0.5f, z + 0.5f, u1, v2,
                 l * (1.0f - a - b - g));
          VERTEX(index, x + 0.5f, y + 0.5f, z - 0.5f, u1, v1,
                 l * (1.0f - a - d - h));
        }
        if (!IS_BLOCKED(x, y - 1, z)) {
          l = 0.75f;

          if (ambientOcclusion) {
            a = IS_BLOCKED(x + 1, y - 1, z) * aoFactor;
            b = IS_BLOCKED(x, y - 1, z + 1) * aoFactor;
            c = IS_BLOCKED(x - 1, y - 1, z) * aoFactor;
            d = IS_BLOCKED(x, y - 1, z - 1) * aoFactor;

            e = IS_BLOCKED(x - 1, y - 1, z - 1) * aoFactor;
            f = IS_BLOCKED(x - 1, y - 1, z + 1) * aoFactor;
            g = IS_BLOCKED(x + 1, y - 1, z + 1) * aoFactor;
            h = IS_BLOCKED(x + 1, y - 1, z - 1) * aoFactor;
          }

          VERTEX(index, x - 0.5f, y - 0.5f, z - 0.5f, u1, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x + 0.5f, y - 0.5f, z + 0.5f, u2, v2,
                 l * (1.0f - a - b - g));
          VERTEX(index, x - 0.5f, y - 0.5f, z + 0.5f, u1, v2,
                 l * (1.0f - c - b - f));

          VERTEX(index, x - 0.5f, y - 0.5f, z - 0.5f, u1, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x + 0.5f, y - 0.5f, z - 0.5f, u2, v1,
                 l * (1.0f - a - d - h));
          VERTEX(index, x + 0.5f, y - 0.5f, z + 0.5f, u2, v2,
                 l * (1.0f - a - b - g));
        }

        if (!IS_BLOCKED(x + 1, y, z)) {
          l = 0.95f;

          if (ambientOcclusion) {
            a = IS_BLOCKED(x + 1, y + 1, z) * aoFactor;
            b = IS_BLOCKED(x + 1, y, z + 1) * aoFactor;
            c = IS_BLOCKED(x + 1, y - 1, z) * aoFactor;
            d = IS_BLOCKED(x + 1, y, z - 1) * aoFactor;

            e = IS_BLOCKED(x + 1, y - 1, z - 1) * aoFactor;
            f = IS_BLOCKED(x + 1, y - 1, z + 1) * aoFactor;
            g = IS_BLOCKED(x + 1, y + 1, z + 1) * aoFactor;
            h = IS_BLOCKED(x + 1, y + 1, z - 1) * aoFactor;
          }

          VERTEX(index, x + 0.5f, y - 0.5f, z - 0.5f, u2, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x + 0.5f, y + 0.5f, z - 0.5f, u2, v2,
                 l * (1.0f - d - a - h));
          VERTEX(index, x + 0.5f, y + 0.5f, z + 0.5f, u1, v2,
                 l * (1.0f - a - b - g));

          VERTEX(index, x + 0.5f, y - 0.5f, z - 0.5f, u2, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x + 0.5f, y + 0.5f, z + 0.5f, u1, v2,
                 l * (1.0f - a - b - g));
          VERTEX(index, x + 0.5f, y - 0.5f, z + 0.5f, u1, v1,
                 l * (1.0f - b - c - f));
        }
        if (!IS_BLOCKED(x - 1, y, z)) {
          l = 0.85f;

          if (ambientOcclusion) {
            a = IS_BLOCKED(x - 1, y + 1, z) * aoFactor;
            b = IS_BLOCKED(x - 1, y, z + 1) * aoFactor;
            c = IS_BLOCKED(x - 1, y - 1, z) * aoFactor;
            d = IS_BLOCKED(x - 1, y, z - 1) * aoFactor;

            e = IS_BLOCKED(x - 1, y - 1, z - 1) * aoFactor;
            f = IS_BLOCKED(x - 1, y - 1, z + 1) * aoFactor;
            g = IS_BLOCKED(x - 1, y + 1, z + 1) * aoFactor;
            h = IS_BLOCKED(x - 1, y + 1, z - 1) * aoFactor;
          }

          VERTEX(index, x - 0.5f, y - 0.5f, z - 0.5f, u1, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x - 0.5f, y + 0.5f, z + 0.5f, u2, v2,
                 l * (1.0f - a - b - g));
          VERTEX(index, x - 0.5f, y + 0.5f, z - 0.5f, u1, v2,
                 l * (1.0f - d - a - h));

          VERTEX(index, x - 0.5f, y - 0.5f, z - 0.5f, u1, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x - 0.5f, y - 0.5f, z + 0.5f, u2, v1,
                 l * (1.0f - b - c - f));
          VERTEX(index, x - 0.5f, y + 0.5f, z + 0.5f, u2, v2,
                 l * (1.0f - a - b - g));
        }

        if (!IS_BLOCKED(x, y, z + 1)) {
          l = 0.9f;

          if (ambientOcclusion) {
            a = IS_BLOCKED(x, y + 1, z + 1) * aoFactor;
            b = IS_BLOCKED(x + 1, y, z + 1) * aoFactor;
            c = IS_BLOCKED(x, y - 1, z + 1) * aoFactor;
            d = IS_BLOCKED(x - 1, y, z + 1) * aoFactor;

            e = IS_BLOCKED(x - 1, y - 1, z + 1) * aoFactor;
            f = IS_BLOCKED(x + 1, y - 1, z + 1) * aoFactor;
            g = IS_BLOCKED(x + 1, y + 1, z + 1) * aoFactor;
            h = IS_BLOCKED(x - 1, y + 1, z + 1) * aoFactor;
          }

          VERTEX(index, x - 0.5f, y - 0.5f, z + 0.5f, u1, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x + 0.5f, y + 0.5f, z + 0.5f, u2, v2,
                 l * (1.0f - a - b - g));
          VERTEX(index, x - 0.5f, y + 0.5f, z + 0.5f, u1, v2,
                 l * (1.0f - a - d - h));

          VERTEX(index, x - 0.5f, y - 0.5f, z + 0.5f, u1, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x + 0.5f, y - 0.5f, z + 0.5f, u2, v1,
                 l * (1.0f - b - c - f));
          VERTEX(index, x + 0.5f, y + 0.5f, z + 0.5f, u2, v2,
                 l * (1.0f - a - b - g));
        }
        if (!IS_BLOCKED(x, y, z - 1)) {
          l = 0.8f;

          if (ambientOcclusion) {
            a = IS_BLOCKED(x, y + 1, z - 1) * aoFactor;
            b = IS_BLOCKED(x + 1, y, z - 1) * aoFactor;
            c = IS_BLOCKED(x, y - 1, z - 1) * aoFactor;
            d = IS_BLOCKED(x - 1, y, z - 1) * aoFactor;

            e = IS_BLOCKED(x - 1, y - 1, z - 1) * aoFactor;
            f = IS_BLOCKED(x + 1, y - 1, z - 1) * aoFactor;
            g = IS_BLOCKED(x + 1, y + 1, z - 1) * aoFactor;
            h = IS_BLOCKED(x - 1, y + 1, z - 1) * aoFactor;
          }

          VERTEX(index, x - 0.5f, y - 0.5f, z - 0.5f, u2, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x - 0.5f, y + 0.5f, z - 0.5f, u2, v2,
                 l * (1.0f - a - d - h));
          VERTEX(index, x + 0.5f, y + 0.5f, z - 0.5f, u1, v2,
                 l * (1.0f - a - b - g));

          VERTEX(index, x - 0.5f, y - 0.5f, z - 0.5f, u2, v1,
                 l * (1.0f - c - d - e));
          VERTEX(index, x + 0.5f, y + 0.5f, z - 0.5f, u1, v2,
                 l * (1.0f - a - b - g));
          VERTEX(index, x + 0.5f, y - 0.5f, z - 0.5f, u1, v1,
                 l * (1.0f - b - c - f));
        }
      }
    }
  }
  return init_mesh(renderer->buffer, index / VERTEX_SIZE, chunk_attrs);
}
