#pragma once

#include "Types.h"

struct GLFWwindow;

typedef struct {
  struct GLFWwindow* gl_window;

} Window;

Window* init_window(i32 width, i32 height, const char* title);
void free_window(Window* window);

i32 window_should_close(Window* window);
void window_set_should_close(Window* window, i32);
void window_swap_buffers(Window* window);
