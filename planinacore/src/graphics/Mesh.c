#include "Mesh.h"

#include <glad/glad.h>
// It should go next for glad capability
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "Logger.h"

Mesh* init_mesh(const f32* buffer, u32 vertices, const i32* attrs) {
  Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
  plog_debug("Init mesh [%p]", mesh);

  mesh->vertices = vertices;
  int vertex_size = 0;
  for (int i = 0; attrs[i]; i++) {
    vertex_size += attrs[i];
  }

  glGenVertexArrays(1, &mesh->vao);
  glGenBuffers(1, &mesh->vbo);

  glBindVertexArray(mesh->vao);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, buffer,
               GL_STATIC_DRAW);

  // attributes
  int offset = 0;
  for (int i = 0; attrs[i]; i++) {
    int size = attrs[i];
    glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE,
                          vertex_size * sizeof(float),
                          (GLvoid*)(offset * sizeof(float)));
    glEnableVertexAttribArray(i);
    offset += size;
  }

  glBindVertexArray(0);

  return mesh;
}
void free_mesh(Mesh* mesh) {
  plog_debug("Free mesh [%p]", mesh);
  glDeleteVertexArrays(1, &mesh->vao);
  glDeleteBuffers(1, &mesh->vbo);
  free(mesh);
}

void mesh_draw(Mesh* mesh, u32 primitive) {
  glBindVertexArray(mesh->vao);
  glDrawArrays(primitive, 0, mesh->vertices);
  glBindVertexArray(0);
}
