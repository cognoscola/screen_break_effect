// Created by alvaregd on 01/12/15.

/**
 * Skybox
 * draws a giant box that surround the camera
 */

#ifndef SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H
#define SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H

#include <GL/glew.h>

#define SKY_VERTEX  "./skybox/skybox.vert"
#define SKY_FRAGMENT "./skybox/skybox.frag"

#define SKY_BACK "./assets/back.png"
#define SKY_BOTTOM "./assets/bottom.png"
#define SKY_FRONT "./assets/front.png"
#define SKY_LEFT "./assets/left.png"
#define SKY_RIGHT "./assets/right.png"
#define SKY_TOP "./assets/top.png"

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


/**
 * Initialize a skybox object. Since there is only one skybox, initial
 * object states are hardcoded. It is safe to call skyboxRender after this
 * out sky: the skybox to initialize
 * in projection_matrix: the scene's projection matrix
 */

void skyInit(Skybox* sky, GLfloat* projection_matrix);
/**
 * Load the textures that will go on the specified skybox
 * Note: Since there is only one skybox, there is only one set of textures and so they are
 * hardcoded in this project
 * out sky: the skybox object to hold the textures
 */

void skyLoadCubeMapTextures(Skybox *sky);

/**
 * Load the shader program for this skyobject
 */

void skyLoadShaderProgram(Skybox *sky);

/** create buffer object **/
void skyCreateVbo(Skybox* sky);

/** create array object **/
void skyCreateVao(Skybox* sky);

/**get shader variables**/
void skyGetUniforms(Skybox* sky);

/**
 * Render the specified skybox object
 * in sky: object to render
 * in camera: scene's camera
 * in isAboceWater: specify if the camera is above water
 * in isRefractionPass: specify if rendering to a refraction framebuffer
 */
void skyRender(Skybox *sky, Camera* camera);

/** update the position and orientation of the sky **/
void skyUpdate(Skybox *sky);

/** clean up buffers **/
void skyCleanUp(Skybox* sky);

#endif //SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H
