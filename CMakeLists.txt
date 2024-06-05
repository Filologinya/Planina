cmake_minimum_required(VERSION 3.12)

project(planina)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_FLAGS "-Wall -O2")

add_executable(planina main.c)

# Добавление подпроекта, указывается имя дирректории
add_subdirectory(planinacore)

# link library *core     
target_link_libraries(planina planinacore)

# create compile_commands.json for vim support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
