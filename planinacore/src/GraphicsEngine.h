#pragma once

#include "InputHandler.h"
#include "ResourceManager.h"
#include "graphics/Window.h"
#include "graphics/Camera.h"

typedef struct {
  Window* window;
  InputHandler* input;
  ResourceManager* resource;
  Camera* camera;
} GraphicsEngine;

GraphicsEngine* init_graphics_engine(int argc, char* argv[]);
void free_graphics_engine(GraphicsEngine* engine);

void engine_run(GraphicsEngine* engine);
