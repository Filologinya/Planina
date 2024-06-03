#pragma once

#include "Types.h"
#include "graphics/Window.h"

typedef struct {
} InputHandler;

InputHandler* init_input_handler(Window* window);
void free_input_handler(InputHandler* input);

// Proceed callbacks for keyboard and mouse
void pull_events(InputHandler* input);

// Toggle cursor active and hidden
void input_toggle_cursor(InputHandler* input, Window* window);

// Check if cursor is moving camera
i8 cursor_moving_camera(InputHandler* input);

// Pressed functions: for keyboard
// return 1 for pressed and 0 for released
i8 pressed(InputHandler* input, i32 keycode);

// Just pressed in last frame
i8 jpressed(InputHandler* input, i32 keycode);

// Clicked functions: for mouse
// return 1 for pressed and 0 for released
i8 clicked(InputHandler* input, i32 button);

// Just pressed in last frame
i8 jclicked(InputHandler* input, i32 button);

// Get mouse delta
f32 inputDeltaX(InputHandler* input);
f32 inputDeltaY(InputHandler* input);

