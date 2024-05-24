#pragma once

#include "InputHandler.h"
#include "graphics/Window.h"

typedef struct {
  Window* window;
  InputHandler *input;
} GraphicsEngine;

GraphicsEngine* init_graphics_engine();
void free_graphics_engine(GraphicsEngine* engine);

void engine_run(GraphicsEngine* engine);
