//
// Created by alvaregd on 30/12/15.
//

#ifndef SCREEN_BREAK_EFFECT_GLASS_H
#define SCREEN_BREAK_EFFECT_GLASS_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>

#define GLASS_DUDV 0
#define GLASS_NORMAL 1

#define GLASS_REFLECTION_WIDTH  320
#define GLASS_REFLECTION_HEIGHT 180

#define GLASS_REFRACTION_WIDTH 1280
#define GLASS_REFRACTION_HEIGHT 120

#define GLASS_VERTEX "/home/alvaregd/Documents/Games/screen_break_effect/glass/glass.vert"
#define GLASS_FRAGMENT "/home/alvaregd/Documents/Games/screen_break_effect/glass/glass.frag"
#define GLASS_GEOMETRY "/home/alvaregd/Documents/Games/screen_break_effect/glass/glass.geom"

#define vertex "/home/alvaregd/Documents/Games/screen_break_effect/glass/sample.vert"
#define fragment "/home/alvaregd/Documents/Games/screen_break_effect/glass/sample.frag"
#define dotgeometry "/home/alvaregd/Documents/Games/screen_break_effect/glass/dot.geom"
#define trigeometry "/home/alvaregd/Documents/Games/screen_break_effect/glass/triangle.geom"


/**
 * Structure to hold informartion about our water surface
 */
struct Glass {
    GLuint reflectionFrameBuffer;
    GLuint reflectionTexture;
    GLuint reflectionDepthBuffer;

    GLuint positionVbo;
    GLuint texCoordVbo;
    GLuint vao;

    GLuint shader;

    GLuint sampleShader;
    GLuint sampleVao;
    GLuint sampleVbo;

    //shader variables
    GLint location_time;
    GLint location_reflectionTexture;
    GLint location_viewMatrix;
    GLint location_modelMatrix;
    GLint location_projMatrix;
    GLint location_projMattrixTest;

    int num_points;
    int num_triangles;

    mat4 modelMatrix;
};

void glassInit(Glass *glass, Window *hardware, GLfloat* proj_mat);
void glassCreateVao(Glass* glass);
GLuint glassCreateFrameBuffer();
GLuint glassCreateTextureAttachment(int width, int height);
GLuint glassCreateDepthBufferAttachment(int width, int height);
void glassUnbindCurrentFrameBuffer(Window * hardware);
void glassBindFrameBufer(GLuint frameBuffer, int width, int height);
void glassGetUniforms(Glass* glass);
void glassRender(Glass* glass, Camera *camera, double time) ;
void glassCleanUp(Glass* glass);

#endif //SCREEN_BREAK_EFFECT_GLASS_H
