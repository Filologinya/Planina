#include "ResourceManager.h"

#include <glad/glad.h>
// It should go next for glad capability
#include <GLFW/glfw3.h>
#include <string.h>

#include "Logger.h"

// should be defined before include
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

ResourceManager* init_resource_manager(char* argv_0, char* resourses,
                                       char* config) {
  ResourceManager* res = (ResourceManager*)malloc(sizeof(ResourceManager));
  plog_debug("Init ResourceManager [%p]", res);

  res->sep = '/';

  u32 sz_exe = strlen(argv_0);
  res->exe_path = (char*)malloc(sz_exe);
  strcpy(res->exe_path, argv_0);

  u32 sz_res = strlen(resourses);
  res->res_path = (char*)malloc(sz_res);
  strcpy(res->res_path, resourses);

  u32 sz_cfg = strlen(config);
  res->config_path = (char*)malloc(sz_cfg);
  strcpy(res->config_path, config);

  return res;
}
void free_resource_manager(ResourceManager* res) {
  plog_debug("Free ResourceManager [%p]", res);

  plog_debug("Free string (%s) [%p]", res->exe_path);
  free(res->exe_path);

  plog_debug("Free string (%s) [%p]", res->res_path);
  free(res->res_path);

  plog_debug("Free string (%s) [%p]", res->config_path);
  free(res->config_path);

  free(res);
}

u32 load_res_texture(ResourceManager* manager, const char* filename,
                     i32* out_width, i32* out_height) {
  u32 sz_path = strlen(manager->res_path);
  u32 sz_name = strlen(filename);
  u32 full_sz = sz_path + 1 + sz_name + 1;

  char* full_filename = (char*)malloc(full_sz);
  memset(full_filename, 0, full_sz);
  plog_debug("Init string [%p]", full_filename, full_filename);
  strcpy(full_filename, manager->res_path);
  full_filename[sz_path] = manager->sep;
  strcpy(full_filename + sz_path + 1, filename);
  plog_trace("Got string (%s)", full_filename);

  i32 width, height, channels;
  u8* img_data = stbi_load(full_filename, &width, &height, &channels, 0);
  plog_debug("Init img_data [%p]", img_data);
  plog_debug("Free string [%p]", full_filename);
  free(full_filename);

  plog_trace("Read image (%s) with size (%dx%d) and channels = %d", filename,
             width, height, channels);
  //for (u32 i = 0; i < width * height - 4; ++i) {
  //  plog_trace("%d: color (%d.%d.%d.%d)", i, img_data[i], img_data[i + 1],
  //             img_data[i + 2], img_data[i + 3]);
  //}

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, (GLvoid*)img_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  plog_debug("Free img_data [%p]", img_data);
  stbi_image_free(img_data);

  *out_width = width;
  *out_height = height;

  plog_trace("Proceed texture (%d) to openGL", texture);
  return texture;
}

char* read_res_file(ResourceManager* manager, const char* filename) {
  u32 sz_path = strlen(manager->res_path);
  u32 sz_name = strlen(filename);
  u32 full_sz = sz_path + 1 + sz_name + 1;

  char* full_filename = (char*)malloc(full_sz);
  memset(full_filename, 0, full_sz);
  plog_debug("Init string [%p]", full_filename, full_filename);
  strcpy(full_filename, manager->res_path);
  full_filename[sz_path] = manager->sep;
  strcpy(full_filename + sz_path + 1, filename);
  plog_trace("Got string (%s)", full_filename);

  FILE* file = fopen(full_filename, "r");
  plog_debug("Free string [%p]", full_filename);
  free(full_filename);
  if (file == NULL) {
    plog_error("Couldn't open file (%s) for reading", filename);
    return NULL;
  }
  plog_debug("Open file (%s) [%p]", filename, file);

  fseek(file, 0L, SEEK_END);
  u32 sz = ftell(file);
  fseek(file, 0L, SEEK_SET);
  char* res = (char*)malloc(sizeof(char) * sz);
  memset(res, 0, sizeof(char) * sz);
  plog_debug("Allocate string [%p]", res);
  u32 code = fread(res, 1, sz, file);
  plog_debug("Close file (%s) [%p]", filename, file);
  plog_trace("Read file (%d):\n%s", sz, res);
  if (code != sz && ferror(file)) {
    plog_error("Error reading from file [%p]", file);
    fclose(file);
    plog_debug("Free string [%p]", res);
    free(res);
    return NULL;
  }
  fclose(file);

  return res;
}
