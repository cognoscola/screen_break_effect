cmake_minimum_required(VERSION 3.3)

project(screen_break_effect)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(CMAKE_MODULE_PATH /usr/local/lib/cmake /usr/local/lib/x86_64-linux-gnu/cmake)
set(CMAKE_PREFIX_PATH /usr/local/lib/cmake/glfw )

set(ASSIMP_INCLUDE_DIR /home/alvaregd/Documents/Games/screen_break_effect/include/)
FIND_LIBRARY(ASSIMP libassimp.so /home/alvaregd/Documents/Games/screen_break_effect/libs/)
if (NOT ASSIMP)
    MESSAGE(SEND_ERROR "Cannot find library libassimp.so")
endif (NOT ASSIMP)

#set(${GLM_DIR} /usr/local/lib/x86_64-linux-gnu/cmake/glm)

find_package (PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package (OpenGL REQUIRED)
#find_package (glfw3 REQUIRED)
find_package (GLM REQUIRED)
find_package (GLEW REQUIRED STATIC)

set(SOURCE_FILES
        main.cpp main.h
        utils/math_utils/maths_funcs.cpp utils/math_utils/maths_funcs.h
        utils/math_utils/quat_funcs.cpp utils/math_utils/quat_funcs.h
        utils/log_utils/logger.cpp utils/log_utils/logger.h
        platform/glfw_launcher.cpp platform/glfw_launcher.h
        platform/hardware.h
        utils/io/shader_loader.cpp utils/io/shader_loader.h
        utils/io/stb_image.cpp utils/io/stb_image.h
        skybox/skybox.h skybox/skybox.cpp
        camera/camera.cpp camera/camera.h
        utils/io/stb_image_write.cpp utils/io/stb_image_write.h
        mesh/mesh.cpp mesh/mesh.h
        utils/io/texture.cpp utils/io/texture.h
        water/water.cpp water/water.h
        input/input.h utils/io/video.cpp
        utils/io/video.h glass/glass.cpp glass/glass.h libs/delaunay.cpp libs/delaunay.h)
#set(SOURCE_FILES main.cpp __add_other_cpp_files_here__)

#set additional libraries
set( ADDITIONAL_LIBS
        -pthread
        -lrt
        -lXinerama
        -lXrandr
        -lXxf86vm
        -lXi
        -lX11
        -lGLEW
        -lGLU
        -lglfw3
        -lm
        )

# Include directories for this project
set(INCLUDE_PATH
        ${OPENGL_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIRS}
        ${ASSIMP_INCLUDE_DIR}
        )

# Libraries needed on all platforms for this project
set(LIBRARIES
        ${X11_LIBRARIES}
        ${ADDITIONAL_LIBS}
        ${GLEW_LIBRARY}
        ${GLFW_STATIC_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${ASSIMP}
        )

add_executable(screen_break_effect ${SOURCE_FILES})


include_directories(${CMAKE_SOURCE_DIR} ${INCLUDE_PATH})
target_link_libraries (screen_break_effect ${LIBRARIES})