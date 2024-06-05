#include "Chunks.h"

#include "Logger.h"
#include "voxel/Chunk.h"
#include <math.h>

Chunks* init_chunks(i32 w, i32 h, i32 d) {
  Chunks* chunks = (Chunks*)malloc(sizeof(Chunks));
  chunks->volume = w * h * d;
  chunks->w = w;
  chunks->h = h;
  chunks->d = d;
  chunks->chunks = (Chunk**)malloc(sizeof(Chunk*) * chunks->volume);
  plog_debug("Init chunks [%p] with array [%p]", chunks, chunks->chunks);

  i32 index = 0;
	for (i32 y = 0; y < h; y++){
		for (i32 z = 0; z < d; z++){
			for (i32 x = 0; x < w; x++, index++){
				chunks->chunks[index] = init_chunk(x, y, z);
			}
		}
	}
  return chunks;
}
void free_chunks(Chunks* chunks) {
  plog_debug("Free chunks [%p] with array [%p]", chunks, chunks->chunks);
  for (u32 i = 0; i < chunks->volume; ++i) {
    free_chunk(chunks->chunks[i]);
  }
  free(chunks->chunks);
  free(chunks);

}

Chunk* chunks_get_chunk(Chunks* c, i32 x, i32 y, i32 z) {
  if (x < 0 || y < 0 || z < 0 || x >= c->w || y >= c->h || z >= c->d)
		return NULL;
	return c->chunks[(y * c->d + z) * c->w + x];
}
Voxel* chunks_get(Chunks* c, i32 x, i32 y, i32 z) {
  i32 cx = x / CHUNK_W;
	i32 cy = y / CHUNK_H;
	i32 cz = z / CHUNK_D;
	if (x < 0) cx--;
	if (y < 0) cy--;
	if (z < 0) cz--;
	if (cx < 0 || cy < 0 || cz < 0 || cx >= c->w || cy >= c->h || cz >= c->d) {
		return NULL;
  }
	Chunk* chunk = c->chunks[(cy * c->d + cz) * c->w + cx];
	i32 lx = x - cx * CHUNK_W;
	i32 ly = y - cy * CHUNK_H;
	i32 lz = z - cz * CHUNK_D;
	return &chunk->voxels[(ly * CHUNK_D + lz) * CHUNK_W + lx];
}
void chunks_set(Chunks* c, i32 id, i32 x, i32 y, i32 z) {
  i32 cx = x / CHUNK_W;
	i32 cy = y / CHUNK_H;
	i32 cz = z / CHUNK_D;
	if (x < 0) cx--;
	if (y < 0) cy--;
	if (z < 0) cz--;
	if (cx < 0 || cy < 0 || cz < 0 || cx >= c->w || cy >= c->h || cz >= c->d)
		return;
	Chunk* chunk = c->chunks[(cy * c->d + cz) * c->w + cx];
	i32 lx = x - cx * CHUNK_W;
	i32 ly = y - cy * CHUNK_H;
	i32 lz = z - cz * CHUNK_D;
	chunk->voxels[(ly * CHUNK_D + lz) * CHUNK_W + lx].id = id;
	chunk->modified = true;

  plog_trace("Set modified for {%d, %d, %d}", cx, cy, cz);
	if (lx == 0 && (chunk = chunks_get_chunk(c, cx-1, cy, cz))) chunk->modified = true;
	if (ly == 0 && (chunk = chunks_get_chunk(c, cx, cy-1, cz))) chunk->modified = true;
	if (lz == 0 && (chunk = chunks_get_chunk(c, cx, cy, cz-1))) chunk->modified = true;

	if (lx == CHUNK_W-1 && (chunk = chunks_get_chunk(c, cx+1, cy, cz))) chunk->modified = true;
	if (ly == CHUNK_H-1 && (chunk = chunks_get_chunk(c, cx, cy+1, cz))) chunk->modified = true;
	if (lz == CHUNK_D-1 && (chunk = chunks_get_chunk(c, cx, cy, cz+1))) chunk->modified = true;
}
Voxel* chunks_ray_cast(Chunks* c, vec3 a, vec3 dir, float maxDist, vec3* end,
                       vec3* norm, vec3* iend) {
  f32 px = a[0];
	f32 py = a[1];
	f32 pz = a[2];

	f32 dx = dir[0];
	f32 dy = dir[1];
	f32 dz = dir[2];

	f32 t = 0.0f;
	i32 ix = floor(px);
	i32 iy = floor(py);
	i32 iz = floor(pz);

	f32 stepx = (dx > 0.0f) ? 1.0f : -1.0f;
	f32 stepy = (dy > 0.0f) ? 1.0f : -1.0f;
	f32 stepz = (dz > 0.0f) ? 1.0f : -1.0f;

	f32 infinity = INFINITY;

	f32 txDelta = (dx == 0.0f) ? infinity : abs(1.0f / dx);
	f32 tyDelta = (dy == 0.0f) ? infinity : abs(1.0f / dy);
	f32 tzDelta = (dz == 0.0f) ? infinity : abs(1.0f / dz);

	f32 xdist = (stepx > 0) ? (ix + 1 - px) : (px - ix);
	f32 ydist = (stepy > 0) ? (iy + 1 - py) : (py - iy);
	f32 zdist = (stepz > 0) ? (iz + 1 - pz) : (pz - iz);

	f32 txMax = (txDelta < infinity) ? txDelta * xdist : infinity;
	f32 tyMax = (tyDelta < infinity) ? tyDelta * ydist : infinity;
	f32 tzMax = (tzDelta < infinity) ? tzDelta * zdist : infinity;

	i32 steppedIndex = -1;

	while (t <= maxDist){
		Voxel* voxel = chunks_get(c, ix, iy, iz);
		if (voxel != NULL && (voxel->id > 0)){
			(*end)[0] = px + t * dx;
			(*end)[1] = py + t * dy;
			(*end)[2] = pz + t * dz;

			(*iend)[0] = (float)ix;
			(*iend)[1] = (float)iy;
			(*iend)[2] = (float)iz;

      (*norm)[0] = (*norm)[1] = (*norm)[2] = 0.0f;
			if (steppedIndex == 0) (*norm)[0] = -stepx;
			if (steppedIndex == 1) (*norm)[1] = -stepy;
			if (steppedIndex == 2) (*norm)[2] = -stepz;
			return voxel;
		}
		if (txMax < tyMax) {
			if (txMax < tzMax) {
				ix += stepx;
				t = txMax;
				txMax += txDelta;
				steppedIndex = 0;
			} else {
				iz += stepz;
				t = tzMax;
				tzMax += tzDelta;
				steppedIndex = 2;
			}
		} else {
			if (tyMax < tzMax) {
				iy += stepy;
				t = tyMax;
				tyMax += tyDelta;
				steppedIndex = 1;
			} else {
				iz += stepz;
				t = tzMax;
				tzMax += tzDelta;
				steppedIndex = 2;
			}
		}
	}
	(*iend)[0] = ix;
	(*iend)[1] = iy;
	(*iend)[2] = iz;

	(*end)[0] = px + t * dx;
	(*end)[1] = py + t * dy;
	(*end)[2] = pz + t * dz;
	(*norm)[0] = (*norm)[1] = (*norm)[2] = 0.0f;
  plog_trace("No raycasted voxel found");
	return NULL;

}

u32 chunks_write(Chunks* c, u8* dest) {
  u32 index = 0;
  for (u32 i = 0; i < c->volume; ++i) {
    Chunk* chunk = c->chunks[i];
    for (u32 j = 0; j < CHUNK_SIZE; ++j, ++index) {
      dest[index] = chunk->voxels[j].id;
    }
  }

  return index;
}

u32 chunks_read(Chunks* c, u8* from) {
  u32 index = 0;
  for (u32 i = 0; i < c->volume; ++i) {
    Chunk* chunk = c->chunks[i];
    for (u32 j = 0; j < CHUNK_SIZE; ++j, ++index) {
      chunk->voxels[j].id = from[index];
    }
    chunk->modified = 1;
  }

  return index;
}

