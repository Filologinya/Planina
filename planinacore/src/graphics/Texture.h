#pragma once

#include "Types.h"
#include "ResourceManager.h"

typedef struct {
  u32 id;
  i32 width;
  i32 height;
} Texture;

Texture* init_texture(ResourceManager* manager, const char* filename);
void free_texture(Texture* texture);

void texture_bind(Texture* texture);

