#pragma once

#include "Types.h"

typedef struct {
  char* exe_path;
  char* res_path;
  char* config_path;
  char sep;
} ResourceManager;

ResourceManager* init_resource_manager(char* argv_0, char* resourses,
                                       char* config);
void free_resource_manager(ResourceManager* manager);

u32 load_res_texture(ResourceManager* manager, const char* filename, int* width,
                     int* height);

char* read_res_file(ResourceManager* manager, const char* filename);
