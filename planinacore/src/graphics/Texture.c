#include "Texture.h"

#include <glad/glad.h>
// It should go next for glad capability
#include <GLFW/glfw3.h>
#include <stdlib.h>

#include "Logger.h"
#include "ResourceManager.h"

Texture* init_texture(ResourceManager* manager, const char* filename) {
  Texture* texture = (Texture*)malloc(sizeof(Texture));
  plog_debug("Init Texture [%p]", texture);
  texture->id =
      load_res_texture(manager, filename, &texture->width, &texture->height);

  return texture;
}
void free_texture(Texture* texture) {
  plog_debug("Free Texture [%p]", texture);
  free(texture);
}

void texture_bind(Texture* texture) {
  glBindTexture(GL_TEXTURE_2D, texture->id);
}
