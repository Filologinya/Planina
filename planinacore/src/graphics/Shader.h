#ifndef GRAPHICS_SHADER_H_
#define GRAPGICS_SHADE_H_

typedef struct {
  unsigned int id;
} Shader;

Shader* init_engine(Arguments* arguments, char** errmsg);
void clear_engine(Shader* engine);

#endif