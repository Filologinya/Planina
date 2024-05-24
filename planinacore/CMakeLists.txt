cmake_minimum_required(VERSION 3.12)

project(planinacore)

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(PUBLIC_INCLUDES
    ${INC_DIR}/Hello.h
    ${INC_DIR}/Logger.h
    ${INC_DIR}/Arguments.h
    ${INC_DIR}/InputHandler.h
    ${INC_DIR}/GraphicsEngine.h
    ${INC_DIR}/graphics/Window.h
)

set(PRIVATE_SOURCES
    ${SRC_DIR}/Hello.c
    ${SRC_DIR}/Logger.c
    ${SRC_DIR}/Arguments.c
    ${SRC_DIR}/InputHandler.c
    ${SRC_DIR}/GraphicsEngine.c
    ${SRC_DIR}/graphics/Window.c
)

set(ALL_SRC
    ${PUBLIC_INCLUDES}
    ${PRIVATE_SOURCES}
)

add_library(planinacore
    STATIC
    ${ALL_SRC}
)

target_include_directories(planinacore
    PUBLIC
    ${SRC_DIR}
)

#=- GLFW -=#
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/glfw)

target_link_libraries(planinacore PRIVATE glfw)

#=- Glad -=#
add_subdirectory(third_party/glad)

target_link_libraries(planinacore PRIVATE glad)