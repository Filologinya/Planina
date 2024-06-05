# Planina

Движок рендера воксельной графики, поддерживаются:
 - Простые эффекты освещения (разное освещение с разных сторон блоков и ambient occlusion)
 - Взаимодействие с 3д сценой (установка и удаление блоков на ЛКМ/ПКМ соответственно)
 - Сохранение и загрузка мира (клавиши 3-4)

## Сборка

Установить зависимости (Ubuntu):
```shell
sudo apt install libglfw3-dev libglfw3 libglew-dev libglm-dev libpng-dev libopenal-dev libluajit-5.1-dev libvorbis-dev
```

Сборка с помощью CMake:
```shell
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

