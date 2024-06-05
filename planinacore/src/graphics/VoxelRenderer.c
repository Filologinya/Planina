#include "VoxelRenderer.h"

#include <stdlib.h>

#include "Logger.h"
#include "voxel/Chunk.h"
#include "voxel/Voxel.h"

const i32 VERTEX_SIZE = 3 + 2 + 1;

const int chunk_attrs[] = {3, 2, 1, 0};  // with null terminator


#define CDIV(X,A) (((X) < 0) ? ((X) / (A) - 1) : ((X) / (A)))
#define LOCAL_NEG(X, SIZE) (((X) < 0) ? ((SIZE)+(X)) : (X))
#define LOCAL(X, SIZE) ((X) >= (SIZE) ? ((X) - (SIZE)) : LOCAL_NEG(X, SIZE))
#define IS_CHUNK(X,Y,Z) (GET_CHUNK(X,Y,Z) != NULL)
#define GET_CHUNK(X,Y,Z) (chunks[((CDIV(Y, CHUNK_H)+1) * 3 + CDIV(Z, CHUNK_D) + 1) * 3 + CDIV(X, CHUNK_W) + 1])

#define VOXEL(X,Y,Z) (GET_CHUNK(X,Y,Z)->voxels[(LOCAL(Y, CHUNK_H) * CHUNK_D + LOCAL(Z, CHUNK_D)) * CHUNK_W + LOCAL(X, CHUNK_W)])
#define IS_BLOCKED(X,Y,Z) ((!IS_CHUNK(X, Y, Z)) || VOXEL(X, Y, Z).id)

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

Mesh* render(VoxelRenderer* renderer, Chunk* chunk, const Chunk** chunks) {
  f32* buffer = renderer->buffer;
  u32 index = 0;
	for (int y = 0; y < CHUNK_H; y++){
		for (int z = 0; z < CHUNK_D; z++){
			for (int x = 0; x < CHUNK_W; x++){
				Voxel vox = chunk->voxels[(y * CHUNK_D + z) * CHUNK_W + x];
				unsigned int id = vox.id;

				if (!id){
					continue;
				}

				float l;
				float uvsize = 1.0f/16.0f;
				float u = (id % 16) * uvsize;
				float v = 1-((1 + id / 16) * uvsize);

				if (!IS_BLOCKED(x,y+1,z)){
					l = 1.0f;
					vertex(buffer, &index, x - 0.5f, y + 0.5f, z - 0.5f, u+uvsize,v, l);
					vertex(buffer, &index, x - 0.5f, y + 0.5f, z + 0.5f, u+uvsize,v+uvsize, l);
					vertex(buffer, &index, x + 0.5f, y + 0.5f, z + 0.5f, u,v+uvsize, l);

					vertex(buffer, &index, x - 0.5f, y + 0.5f, z - 0.5f, u+uvsize,v, l);
					vertex(buffer, &index, x + 0.5f, y + 0.5f, z + 0.5f, u,v+uvsize, l);
					vertex(buffer, &index, x + 0.5f, y + 0.5f, z - 0.5f, u,v, l);
				}
				if (!IS_BLOCKED(x,y-1,z)){
					l = 0.75f;
					vertex(buffer, &index, x - 0.5f, y - 0.5f, z - 0.5f, u,v, l);
					vertex(buffer, &index, x + 0.5f, y - 0.5f, z + 0.5f, u+uvsize,v+uvsize, l);
					vertex(buffer, &index, x - 0.5f, y - 0.5f, z + 0.5f, u,v+uvsize, l);

					vertex(buffer, &index, x - 0.5f, y - 0.5f, z - 0.5f, u,v, l);
					vertex(buffer, &index, x + 0.5f, y - 0.5f, z - 0.5f, u+uvsize,v, l);
					vertex(buffer, &index, x + 0.5f, y - 0.5f, z + 0.5f, u+uvsize,v+uvsize, l);
				}

				if (!IS_BLOCKED(x+1,y,z)){
					l = 0.95f;
					vertex(buffer, &index, x + 0.5f, y - 0.5f, z - 0.5f, u+uvsize,v, l);
					vertex(buffer, &index, x + 0.5f, y + 0.5f, z - 0.5f, u+uvsize,v+uvsize, l);
					vertex(buffer, &index, x + 0.5f, y + 0.5f, z + 0.5f, u,v+uvsize, l);

					vertex(buffer, &index, x + 0.5f, y - 0.5f, z - 0.5f, u+uvsize,v, l);
					vertex(buffer, &index, x + 0.5f, y + 0.5f, z + 0.5f, u,v+uvsize, l);
					vertex(buffer, &index, x + 0.5f, y - 0.5f, z + 0.5f, u,v, l);
				}
				if (!IS_BLOCKED(x-1,y,z)){
					l = 0.85f;
					vertex(buffer, &index, x - 0.5f, y - 0.5f, z - 0.5f, u,v, l);
					vertex(buffer, &index, x - 0.5f, y + 0.5f, z + 0.5f, u+uvsize,v+uvsize, l);
					vertex(buffer, &index, x - 0.5f, y + 0.5f, z - 0.5f, u,v+uvsize, l);

					vertex(buffer, &index, x - 0.5f, y - 0.5f, z - 0.5f, u,v, l);
					vertex(buffer, &index, x - 0.5f, y - 0.5f, z + 0.5f, u+uvsize,v, l);
					vertex(buffer, &index, x - 0.5f, y + 0.5f, z + 0.5f, u+uvsize,v+uvsize, l);
				}

				if (!IS_BLOCKED(x,y,z+1)){
					l = 0.9f;
					vertex(buffer, &index, x - 0.5f, y - 0.5f, z + 0.5f, u,v, l);
					vertex(buffer, &index, x + 0.5f, y + 0.5f, z + 0.5f, u+uvsize,v+uvsize, l);
					vertex(buffer, &index, x - 0.5f, y + 0.5f, z + 0.5f, u,v+uvsize, l);

					vertex(buffer, &index, x - 0.5f, y - 0.5f, z + 0.5f, u,v, l);
					vertex(buffer, &index, x + 0.5f, y - 0.5f, z + 0.5f, u+uvsize,v, l);
					vertex(buffer, &index, x + 0.5f, y + 0.5f, z + 0.5f, u+uvsize,v+uvsize, l);
				}
				if (!IS_BLOCKED(x,y,z-1)){
					l = 0.8f;
					vertex(buffer, &index, x - 0.5f, y - 0.5f, z - 0.5f, u+uvsize,v, l);
					vertex(buffer, &index, x - 0.5f, y + 0.5f, z - 0.5f, u+uvsize,v+uvsize, l);
					vertex(buffer, &index, x + 0.5f, y + 0.5f, z - 0.5f, u,v+uvsize, l);

					vertex(buffer, &index, x - 0.5f, y - 0.5f, z - 0.5f, u+uvsize,v, l);
					vertex(buffer, &index, x + 0.5f, y + 0.5f, z - 0.5f, u,v+uvsize, l);
					vertex(buffer, &index, x + 0.5f, y - 0.5f, z - 0.5f, u,v, l);
				}
			}
		}
	}
  return init_mesh(renderer->buffer, index / VERTEX_SIZE, chunk_attrs);
}
