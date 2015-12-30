//
// Created by alvaregd on 26/11/15.
//

#ifndef FPS_STYLE_ROOM_QUAT_FUNCS_H
#define FPS_STYLE_ROOM_QUAT_FUNCS_H

#include <GL/gl.h>
#include "maths_funcs.h"

#define PITCH 0
#define YAW  1
#define ROLL  2

void create_versor (float* q, float a, float x, float y, float z);
void quat_to_mat4 (float* m, float* q) ;
void normalise_quat (float* q);
void mult_quat_quat (float* result, float* r, float* s);

void calculateRotationMatrix(GLfloat angle, mat4 *matrix, int type);
#endif //FPS_STYLE_ROOM_QUAT_FUNCS_H
