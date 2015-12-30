// Created by alvaregd on 01/12/15.

#ifndef SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H
#define SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H

#include <GL/glew.h>

#define SKY_VERTEX  "/home/alvaregd/Documents/Games/screen_break_effect/skybox/skybox.vert"
#define SKY_FRAGMENT "/home/alvaregd/Documents/Games/screen_break_effect/skybox/skybox.frag"

#define SKY_BACK "/home/alvaregd/Documents/Games/screen_break_effect/assets/back.png"
#define SKY_BOTTOM "/home/alvaregd/Documents/Games/screen_break_effect/assets/bottom.png"
#define SKY_FRONT "/home/alvaregd/Documents/Games/screen_break_effect/assets/front.png"
#define SKY_LEFT "/home/alvaregd/Documents/Games/screen_break_effect/assets/left.png"
#define SKY_RIGHT "/home/alvaregd/Documents/Games/screen_break_effect/assets/right.png"
#define SKY_TOP "/home/alvaregd/Documents/Games/screen_break_effect/assets/top.png"

#define VALUE 100.0f;
#define SKY_MAP_VERTEX_COUNT 36

/**
 * A structure to hold information regarding the sky.
 * This includes various ids for textures, shader programm and shader variables
 * a model matrix to describe the object in the scene
 */
struct Skybox{
    GLuint shader;
    GLuint texture;
    GLuint vao;
    GLuint vbo;
    GLint location_projection_mat;
    GLint location_view_mat;
    GLint location_model_mat;
    mat4 modelMatrix;

    GLfloat angle;
};

void skyInit(Skybox* sky, GLfloat* projection_matrix);
void skyLoadCubeMapTextures(Skybox *sky);
void skyLoadShaderProgram(Skybox *sky);

/** create buffer object **/
void skyCreateVbo(Skybox* sky);

/** create array object **/
void skyCreateVao(Skybox* sky);

/**get shader variables**/
void skyGetUniforms(Skybox* sky);

/**
 * Render the object
 * in - sky object
 * in - camera
 */
void skyRender(Skybox *sky, Camera* camera);

/** update the position and orientation of the sky **/
void skyUpdate(Skybox *sky);

/** clean up buffers **/
void skyCleanUp(Skybox* sky);

#endif //SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H
