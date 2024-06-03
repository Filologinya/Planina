#include "Shader.h"

#include <glad/glad.h>
// It should go next for glad capability
#include <GLFW/glfw3.h>
#include <stdlib.h>

#include "Logger.h"
#include "ResourceManager.h"
#include "Types.h"

Shader* init_shader(ResourceManager* res, const char* vertex_name,
                    const char* fragment_name) {
  // Process vertex shader
  GLint success;
  GLchar infoLog[512];

  GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
  char* vertex_code = read_res_file(res, vertex_name);
  glShaderSource(vertex, 1, (const char* const*)&vertex_code, NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  plog_debug("Free str [%p]", vertex_code);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    plog_error("Couldn't init vertex shader: %s", infoLog);
    return NULL;
  }

  // Process fragment shader
  GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
  char* fragment_code = read_res_file(res, fragment_name);
  glShaderSource(fragment, 1, (const char* const*)&fragment_code, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  plog_debug("Free str [%p]", fragment_code);
  free(fragment_code);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    plog_error("Couldn't init fragment shader: %s", infoLog);

    return NULL;
  }

  // Create shader programm
  GLuint id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);

  glGetProgramiv(id, GL_LINK_STATUS, &success);
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    plog_error("Couldn't link shader program: %s", infoLog);

    return NULL;
  }

  plog_trace("Created shader from vertex (%s) and fragment (%s) files",
             vertex_name, fragment_name);

  Shader* shader = (Shader*)malloc(sizeof(Shader));
  shader->id = id;
  plog_debug("Create Shader (%d) [%p]", shader->id, shader);
  return shader;
}

void free_shader(Shader* shader) {
  plog_debug("Free Shader (%d) [%p]", shader->id, shader);
  glDeleteProgram(shader->id);
  free(shader);
}

void shader_use(Shader* shader) { glUseProgram(shader->id); }

void shader_uniform_matrix(Shader* shader, const char* name, mat4 matrix) {
  GLuint transform_loc = glGetUniformLocation(shader->id, name);
  glUniformMatrix4fv(transform_loc, 1, GL_FALSE, (const float*)matrix);

}
