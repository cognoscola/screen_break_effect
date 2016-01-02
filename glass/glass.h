//
// Created by alvaregd on 30/12/15.
//

#ifndef SCREEN_BREAK_EFFECT_GLASS_H
#define SCREEN_BREAK_EFFECT_GLASS_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>

#define GLASS_DUDV_FILE "/home/alvaregd/Documents/Games/screen_break_effect/assets/waterDUDV.png"
#define GLASS_NORMALMAP_FILE "/home/alvaregd/Documents/Games/screen_break_effect/assets/normalMap.png"

#define GLASS_DUDV 0
#define GLASS_NORMAL 1

#define GLASS_REFLECTION_WIDTH  320
#define GLASS_REFLECTION_HEIGHT 180

#define GLASS_REFRACTION_WIDTH 1280
#define GLASS_REFRACTION_HEIGHT 120

#define GLASS_VERTEX "/home/alvaregd/Documents/Games/screen_break_effect/glass/glass.vert"
#define GLASS_FRAGMENT "/home/alvaregd/Documents/Games/screen_break_effect/glass/glass.frag"

#define WAVE_SPEED 0.03

#define NUM_POINTS 5

/**
 * Structure to hold informartion about our water surface
 */
struct Glass {

    GLuint reflectionFrameBuffer;
    GLuint reflectionTexture;
    GLuint reflectionDepthBuffer;
    GLuint refractionFrameBuffer;
    GLuint refractionTexture;
    GLuint refractionDepthTexture;

    GLuint dudvTexture;
    GLuint normalMapTexture;

    GLuint positionVbo;
    GLuint texCoordVbo;
    GLuint vao;

    GLuint shader;

    //shader variables
    GLint location_reflectionTexture;
    GLint location_refractionTexture;
    GLint location_dudv;
    GLint location_viewMatrix;
    GLint location_modelMatrix;
    GLint location_projMatrix;
    GLint location_moveFactor;
    GLint location_cameraPosition;
    GLint location_normalMap;
    GLint location_lightColour;
    GLint location_lightPosition;
    GLint location_depthMap;
    int location_dots[NUM_POINTS];

    mat4 modelMatrix;
    GLfloat glassHeight;
    GLfloat reflectionDistance;
    double moveFactor;
};

void glassInit(Glass *glass, Window *hardware, GLfloat* proj_mat);
void glassLoadTexture(Glass* glass, const char* name, int type);
void glassCreateVao(Glass* glass);
GLuint glassCreateFrameBuffer();
GLuint glassCreateTextureAttachment(int width, int height);
GLuint glassCreateDepthTextureAttachment(int width, int height);
GLuint glassCreateDepthBufferAttachment(int width, int height);
void glassUnbindCurrentFrameBuffer(Window * hardware);
void glassBindFrameBufer(GLuint frameBuffer, int width, int height);
void glassGetUniforms(Glass* glass);
void glassUpdate(Glass *glass);
void glassRender(Glass* glass, Camera *camera);
void glassCleanUp(Glass* glass);


#endif //SCREEN_BREAK_EFFECT_GLASS_H
