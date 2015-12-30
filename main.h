//
// Created by alvaregd on 27/11/15.
//
#ifndef SIMPLE_LEVEL_EDITOR_MAIN_H_H
#define SIMPLE_LEVEL_EDITOR_MAIN_H_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <utils/math_utils/maths_funcs.h>
#include <utils/math_utils/quat_funcs.h>
#include <utils/log_utils/logger.h>
#include <utils/io/shader_loader.h>
#include <array>
#include <vector>
#include "camera/camera.h"

#define PITCH 0
#define YAW  1
#define ROLL  2


static Input input;
static Camera camera;

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
#endif //SIMPLE_LEVEL_EDITOR_`_H_H
