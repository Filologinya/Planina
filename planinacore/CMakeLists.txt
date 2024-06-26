cmake_minimum_required(VERSION 3.12)

project(planinacore)

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(PUBLIC_INCLUDES
    ${INC_DIR}/Logger.h
    ${INC_DIR}/Arguments.h
    ${INC_DIR}/InputHandler.h
    ${INC_DIR}/GraphicsEngine.h
    ${INC_DIR}/graphics/Camera.h
    ${INC_DIR}/graphics/Window.h
    ${INC_DIR}/graphics/Shader.h
    ${INC_DIR}/graphics/Texture.h
    ${INC_DIR}/graphics/Mesh.h
    ${INC_DIR}/graphics/VoxelRenderer.h
    ${INC_DIR}/voxel/Chunk.h
    ${INC_DIR}/voxel/Chunks.h
    ${INC_DIR}/ResourceManager.h
)

set(PRIVATE_SOURCES
    ${SRC_DIR}/Logger.c
    ${SRC_DIR}/Arguments.c
    ${SRC_DIR}/InputHandler.c
    ${SRC_DIR}/GraphicsEngine.c
    ${SRC_DIR}/graphics/Window.c
    ${SRC_DIR}/graphics/Shader.c
    ${SRC_DIR}/graphics/Texture.c
    ${SRC_DIR}/graphics/Camera.c
    ${SRC_DIR}/graphics/Mesh.c
    ${SRC_DIR}/graphics/VoxelRenderer.c
    ${SRC_DIR}/voxel/Chunk.c
    ${SRC_DIR}/voxel/Chunks.c
    ${SRC_DIR}/ResourceManager.c
)

set(ALL_SRC
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

#=- GLM -=#
add_subdirectory(third_party/cglm)
target_link_libraries(planinacore PUBLIC cglm)

#=- stb -=#
# just include all headers as it is
target_include_directories(planinacore
    PRIVATE
    third_party
)

#=- Resources -=#
add_custom_command(TARGET planinacore POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/res 
                       ${CMAKE_BINARY_DIR}/res)
