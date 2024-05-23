cmake_minimum_required(VERSION 3.12)

project(planinacore)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(PUBLIC_INCLUDES
    ${SRC_DIR}/Hello.h
    ${SRC_DIR}/Logger.h
    ${SRC_DIR}/Arguments.h
)

set(PRIVATE_SOURCES
    ${SRC_DIR}/Hello.c
    ${SRC_DIR}/Logger.c
    ${SRC_DIR}/Arguments.c
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