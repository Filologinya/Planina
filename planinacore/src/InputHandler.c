#include "InputHandler.h"

#include <glad/glad.h>
// It should go next for glad capability
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>

#include "Logger.h"
#include "graphics/Window.h"

static i8 input_handler_inited = 0;

static const i32 keys_size = 1032;
static const i32 mouse_button_start = 1024;

// 1 for pressed and 0 for released
static i8* s_keys;
static u32* s_frames;
static u32 s_current_frame;
static f32 s_deltaX;
static f32 s_deltaY;
static f32 s_x;
static f32 s_y;
static i8 s_cursor_locked;
static i8 s_cursor_started;

void glfw_cursor_position_callback(GLFWwindow* window, f64 xpos, f64 ypos) {
  if (s_cursor_started == 1) {
    s_deltaX += xpos - s_x;
    s_deltaY += ypos - s_y;
  } else {
    s_cursor_started = 1;
  }
  s_x = xpos;
  s_y = ypos;
}

void glfw_key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action,
                       i32 mode) {
  if (action == GLFW_PRESS) {
    s_keys[key] = 1;
    s_frames[key] = s_current_frame;
  } else if (action == GLFW_RELEASE) {
    s_keys[key] = 0;
    s_frames[key] = s_current_frame;
  }
}

void glfw_mouse_button_callback(GLFWwindow* window, i32 button, i32 action,
                                i32 mode) {
  if (action == GLFW_PRESS) {
    s_keys[mouse_button_start + button] = 1;
    s_frames[mouse_button_start + button] = s_current_frame;
  } else if (action == GLFW_RELEASE) {
    s_keys[mouse_button_start + button] = 0;
    s_frames[mouse_button_start + button] = s_current_frame;
  }
}

InputHandler* init_input_handler(Window* window) {
  if (input_handler_inited != 0) {
    plog_warn("%s", "Can't init more than one InputHandler");
    return NULL;
  }
  InputHandler* input = (InputHandler*)malloc(sizeof(InputHandler));
  s_keys = (i8*)malloc(sizeof(i8) * keys_size);
  s_frames = (u32*)malloc(sizeof(u32) * keys_size);
  plog_debug("Create InputHandler [%p] with keys [%p] and frames [%p]", input,
             s_keys, s_frames);

  memset(s_keys, 0, keys_size * sizeof(i8));
  memset(s_frames, 0, keys_size * sizeof(i8));
  s_current_frame = 0;

  s_deltaX = 0.0f;
  s_deltaY = 0.0f;
  s_x = 0.0f;
  s_y = 0.0f;
  s_cursor_locked = 0;
  s_cursor_started = 0;

  glfwSetKeyCallback(window->gl_window, glfw_key_callback);
  glfwSetMouseButtonCallback(window->gl_window, glfw_mouse_button_callback);
  glfwSetCursorPosCallback(window->gl_window, glfw_cursor_position_callback);

  input_handler_inited = 1;
  return input;
}
void free_input_handler(InputHandler* input) {
  plog_debug("Free InputHandler [%p] with keys [%p] and frames [%p]", input,
             s_keys, s_frames);
  free(s_keys);
  free(s_frames);
  free(input);
  input_handler_inited = 0;
}

void pull_events(InputHandler* input) {
  ++s_current_frame;
  s_deltaX = 0.0f;
  s_deltaY = 0.0f;
  glfwPollEvents();
}

void input_toggle_cursor(InputHandler* input, Window* window) {
  s_cursor_locked = !s_cursor_locked;
  window_set_cursor_mode(
      window, s_cursor_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

i8 cursor_moving_camera(InputHandler* input) {
  return s_cursor_locked == 1;
}

i8 pressed(InputHandler* input, i32 keycode) {
  // keycode doesn't fit into keyboard size
  if (!(0 <= keycode && keycode < mouse_button_start)) {
    plog_warn("Wrong keycode (%d) for keyboard", keycode);
    return 0;
  }
  return s_keys[keycode];
}

i8 jpressed(InputHandler* input, i32 keycode) {
  // keycode doesn't fit into keyboard size
  if (!(0 <= keycode && keycode < mouse_button_start)) {
    plog_warn("Wrong keycode (%d) for keyboard", keycode);
    return 0;
  }

  // If time when we pressed keycode is s_current_frame
  return s_keys[keycode] && (s_frames[keycode] == s_current_frame);
}

i8 clicked(InputHandler* input, i32 button) {
  i32 index = button + mouse_button_start;

  // button doesn't fit into mouse values
  if (!(mouse_button_start <= index && index < keys_size)) {
    plog_warn("wrong button (%d) for mouse", index);
    return 0;
  }
  return s_keys[index];
}

i8 jclicked(InputHandler* input, i32 button) {
  i32 index = button + mouse_button_start;

  // button doesn't fit into mouse values
  if (!(mouse_button_start <= index && index < keys_size)) {
    plog_warn("wrong button (%d) for mouse", index);
    return 0;
  }
  return s_keys[index] && (s_frames[index] == s_current_frame);
}

f32 inputDeltaX(InputHandler* input) { return s_deltaX; }
f32 inputDeltaY(InputHandler* input) { return s_deltaY; }
