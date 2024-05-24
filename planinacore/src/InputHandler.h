#pragma once

#include "Types.h"
#include "graphics/Window.h"

typedef struct {
} InputHandler;

InputHandler* init_input_handler(Window* window);
void free_input_handler(InputHandler* input);

// Proceed callbacks for keyboard and mouse
void pull_events(InputHandler* input);

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
