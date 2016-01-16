//
// Created by alvaregd on 30/12/15.
//
/**
 * Breaking Glass
 *
 * As seen in Final Fantasy X by Square Enix, the glass objects captures a single frame and
 * places it on a quad made out of many triangles. This quad is this animated out of the screen
 * You can use anywhere
 *
 */

#ifndef SCREEN_BREAK_EFFECT_GLASS_H
#define SCREEN_BREAK_EFFECT_GLASS_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>

#define GLASS_REFLECTION_WIDTH  320
#define GLASS_REFLECTION_HEIGHT 180

#define GLASS_VERTEX "/home/alvaregd/Documents/Games/screen_break_effect/glass/glass.vert"
#define GLASS_FRAGMENT "/home/alvaregd/Documents/Games/screen_break_effect/glass/glass.frag"
#define GLASS_GEOMETRY "/home/alvaregd/Documents/Games/screen_break_effect/glass/glass.geom"

#define vertex "/home/alvaregd/Documents/Games/screen_break_effect/glass/debug.vert"
#define fragment "/home/alvaregd/Documents/Games/screen_break_effect/glass/debug.frag"
#define dotgeometry "/home/alvaregd/Documents/Games/screen_break_effect/glass/dot.geom"
#define trigeometry "/home/alvaregd/Documents/Games/screen_break_effect/glass/triangle.geom"

#define SCREEN_VERTEX "/home/alvaregd/Documents/Games/screen_break_effect/glass/screen.vert"
#define SCREEN_FRAGMENT "/home/alvaregd/Documents/Games/screen_break_effect/glass/screen.frag"

#define DIMENSIONS 3
#define POINTS 61
#define MAX_TRIANGLES 110

/** describes glass's motion through time*/
struct GlassTransformation{

    vec3* posKeys;        //describes object's position at every key frame
    versor*rotKeys;       //describes object's orientation at every key frame
    vec3* scaleKeys;      //describes object's scale at every key frame
    double* posKeyTimes;  //key frame times for the position key frames
    double* rotKeyTimes;  //key frame times for the rotation key frames
    double* scaKeyTimes;  //key frame times for the scale key frames
    int numPosKeys;
    int numRotKeys;
    int numScaKeys;

    double animationDuration;//time it takes for animation to complete
};

/**
 * Structure to hold information about our water surface
 */
struct Glass {

    GLuint framebuffer; //for holding one frame
    GLuint framebufferTexture; //for getting the frame as a texture
    GLuint depthBuffer;  //for getting depth information

    //glass stuff
    GLuint triangleIdVbo; //for indexing between triangles
    GLuint positionVbo;   //don't really need, but we keep just in case
    GLuint texCoordVbo;   //don't really need, but we keep just in case
    GLuint vao;           //for getting our rendered object information
    GLuint shader;

    //screen stuff
    GLuint screenShader;
    GLuint screenVao;
    GLuint screenPositionVbo;
    GLint location_screen_proj_matrix;
    GLint location_alpha;   //change the transparency of the screen
    GLint location_colour;  //change the colour of the screen

    //debug stuff
    GLuint debugShader;     //helps visualize the triangles without a texture
    GLuint debugVao;
    GLuint debugVbo;

    GLint location_frameTexture; //points to our texture location
    GLint location_viewMatrix;
    GLint location_projMatrix;
    GLint location_projMattrixTest;

    int location_model_matrices[MAX_TRIANGLES]; //shader locations for each triangle's model matrix
    mat4* modelMats;                            //model matrices for each triangle
    int num_points;                             //number of points to create
    int num_triangles;                          //number of triangles created
    GlassTransformation* transformations;       //transformations for each triangle
    double transitionTime = 0.0;                //animation direction
};

/** Initialize the glass objects state. This function will perform
 * everything that is needed to start using the glass object. It is safe
 * to call glassRender after calling this function
 * out glass: the glass object to initialize
 * in window: the window to render the glass in
 * in proj_mat: a projection matrix */
void glassInit(Glass *glass, Window *window, GLfloat* proj_mat);

/** creates the vao and its vbos for the glass */
void glassCreateVao(Glass* glass);

/** creates a frame buffer object
 * returns the id of the fbo*/
GLuint glassCreateFrameBuffer();

/** makes it so that we can fetch a framebuffer as a texture
 * in width: width in pixels of the texture
 * in height: height in pixels of the texture*/
GLuint glassCreateTextureAttachment(int width, int height);
/** makes it so that we can fetch a framebuffer's depth information
 * * in width: width in pixels of the texture
 * in height: height in pixels of the texture*/
GLuint glassCreateDepthBufferAttachment(int width, int height);
/** Generate random transformations for each piece of glass*/
void glassCreateShardTransformations(Glass* glass);

/**
 * Binds to a default framebuffer if the default framebuffer isn't
 * already bounded to.
 * in window: is the window containing the opengl context
 */
void glassUnbindCurrentFrameBuffer(Window * hardware);

/**
 * Binds to the specified frame buffer
 * in framebuffer: the framebuffer to bind to
 * in width: the width to change the viewport to
 * in height: the height to change the viewport to */
void glassBindFrameBufer(GLuint frameBuffer, int width, int height);

/**
 * Get the glass shader's uniform location
 */

void glassGetUniforms(Glass* glass);
/**
 * Render the glass object to the scene. You should probably call this every frame
 * in glass: the glass object to render
 * in camera: so that we can apply a viewmatrix
 * in elapsedseconds: number of seconds passed since begining of the program
 */
void glassRender(Glass* glass, Camera *camera, double elapsedSeconds);

/**
 * delete the glass object and free its memory
 */
void glassCleanUp(Glass* glass);

#endif //SCREEN_BREAK_EFFECT_GLASS_H
