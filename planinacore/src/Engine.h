#pragma once

// структура движка со всеми компонентами
typedef struct {
  ResourceManager* resource;
  InputHandler* input;
  GraphicsEngine* graphics_engine;
} Engine;

// (1-2) Создание и освобождение памяти
Engine* init_engine(Arguments* arguments, char** errmsg);
void clear_engine(Engine* engine);

// (3) Единственный метод: запуск
void start(Engine* engine);
