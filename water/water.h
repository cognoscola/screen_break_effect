//
// Created by alvaregd on 06/12/15.
//
/**
 * Water
 *
 * This object is a flat quad with normal (0, 1, 0 ) that has a shader attachment that draws light effects
 * on it to make it look like water.
 */

#ifndef WATER_REFLECTION_WATER_H
#define WATER_REFLECTION_WATER_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>

#define DUDV_FILE "./assets/waterDUDV.png"
#define NORMALMAP_FILE "./assets/normalMap.png"

#define DUDV 0
#define NORMAL 1

#define REFLECTION_WIDTH  320
#define REFLECTION_HEIGHT 180

#define REFRACTION_WIDTH 1280
#define REFRACTION_HEIGHT 120

#define WATER_VERTEX "./water/water.vert"
#define WATER_FRAGMENT "./water/water.frag"

#define WAVE_SPEED 0.03

/**
 * Structure to hold informartion about our water surface
 */
struct Water {

    GLuint reflectionFrameBuffer;   //reflection framebuffer id
    GLuint reflectionTexture;       //reflection texture id
    GLuint reflectionDepthBuffer;   //reflection depth buffer id
    GLuint refractionFrameBuffer;   //refraction framebuffer id
    GLuint refractionTexture;       //refraction texture id
    GLuint refractionDepthTexture;  //refraction depth texture id

    GLuint dudvTexture;             //depth texture id
    GLuint normalMapTexture;        //normal texture id

    GLuint positionVbo;
    GLuint texCoordVbo;
    GLuint vao;

    GLuint shader;                  //shader id

    //shader variables
    GLint location_reflectionTexture;
    GLint location_refractionTexture;
    GLint location_dudv;
    GLint location_viewMatrix;
    GLint location_modelMatrix;
    GLint location_projMatrix;
    GLint location_moveFactor;      //value that describes a texture's displacement on the quad
    GLint location_cameraPosition;  //value used to calculate vectors for light effects
    GLint location_normalMap;
    GLint location_lightColour;     //specular light colour
    GLint location_lightPosition;   //light source position
    GLint location_depthMap;

    mat4 modelMatrix;
    GLfloat waterHeight;
    GLfloat reflectionDistance;
    double moveFactor;
};

/**
 * Initialize the specified water object. Because there is only one
 * water object in this project, the initial states are hardcoded
 * out water: object to be initialized
 * in window: the window containing the opengl instance
 * in proj_mat : the projection matrix
 */
void waterInit(Water *water, Window *hardware, GLfloat* proj_mat);

/**
 * Load a specified texture effect for this water
 * out water: the water object that will hold the texture
 * in name: name of the texture file
 * in type: one of either DUDV or NORMAL
 */
void waterLoadTexture(Water* water, const char* name, int type);

/**
 * create a vao for the water quad
 */
void waterCreateVao(Water* water);

/**
 * create a frame buffer
 * returns the id of the frame buffer
 */
GLuint createFrameBuffer();

/**
 * create a texture attachment for the currently bound framebuffer
 * in width: in pixels of the texture
 * in height: in pixels of the texture
 */

GLuint createTextureAttachment(int width, int height);

/**
 * create a depth texture attachment for the currently bound framebuffer
 * in width: in pixels of the texture
 * in height: in pixels of the texture
 */
GLuint createDepthTextureAttachment(int width, int height);


/**
 * create a depth buffer attachment for the currently bound framebuffer
 * in width: in pixels of the texture
 * in height: in pixels of the texture
 */
GLuint createDepthBufferAttachment(int width, int height);


/**
 * Bind to the default buffer if it is not already bound.
 */
void unbindCurrentFrameBuffer(Window * hardware);

/**
 * bind to the specified frame buffer
 * in width: of the viewport
 * in height of the viewport
 */
void bindFrameBufer(GLuint frameBuffer, int width, int height);

/** get uniform locations of the shader within the specified water object */
void waterGetUniforms(Water* water);

/* update the specified water object **/
void waterUpdate(Water *water);


/** render the specified water object, be sure to call waterInit before calling this function
 * in water: the object to render
 * in camera: the camera for the scene
 */
void waterRender(Water* water, Camera *camera);

/** free memory taken up by the water object */
void waterCleanUp(Water* water);
#endif //WATER_REFLECTION_WATER_H
