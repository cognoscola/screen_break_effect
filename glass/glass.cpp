//
// Created by alvaregd on 30/12/15.
//
#include <math.h>
#include <utils/io/texture.h>
#include <malloc.h>
#include <GL/glew.h>
#include <platform/glfw_launcher.h>
#include <utils/io/shader_loader.h>
#include <utils/math_utils/quat_funcs.h>
#include <camera/camera.h>
#include "glass.h"

void glassInit(Glass *glass, Window *hardware, GLfloat* proj_mat) {

    //create texture objects for glass effects
//    glassLoadTexture(glass, GLASS_DUDV_FILE, GLASS_DUDV);
//    glassLoadTexture(glass, GLASS_NORMALMAP_FILE, GLASS_NORMAL);
    glassCreateVao(glass);

    //create frame buffer stuff
    glass->reflectionFrameBuffer = glassCreateFrameBuffer();
    glass->reflectionTexture = glassCreateTextureAttachment(GLASS_REFLECTION_WIDTH, GLASS_REFLECTION_HEIGHT);
    glass->reflectionDepthBuffer = glassCreateDepthBufferAttachment(GLASS_REFLECTION_WIDTH, GLASS_REFLECTION_HEIGHT);
    glassUnbindCurrentFrameBuffer(hardware);

//    glass->refractionFrameBuffer = glassCreateFrameBuffer();
//    glass->refractionTexture = glassCreateTextureAttachment(GLASS_REFRACTION_WIDTH, GLASS_REFRACTION_HEIGHT);
//    glass->refractionDepthTexture = glassCreateDepthTextureAttachment(GLASS_REFRACTION_WIDTH, GLASS_REFRACTION_HEIGHT);
//    glassUnbindCurrentFrameBuffer(hardware);

    //create shader
    glass->shader = create_programme_from_files(GLASS_VERTEX, GLASS_FRAGMENT);
    glUseProgram(glass->shader);
    glassGetUniforms(glass);

    //set initial shader settings
    glUniformMatrix4fv(glass->location_projMatrix, 1, GL_FALSE, proj_mat);
//    glUniform3f(glass->location_lightColour , 1.0f,1.0f,1.0f);
//    glUniform3f(glass->location_lightPosition , 50.0f,100.0f,50.0f);
    glUniform1i(glass->location_reflectionTexture, 0 );
//    glUniform1i(glass->location_refractionTexture,1 );
//    glUniform1i(glass->location_dudv,2 );
//    glUniform1i(glass->location_normalMap,3);
//    glUniform1i(glass->location_depthMap,4);

    //calculate initial glass position/orientation
//    glass->glassHeight = 5.0f;
//    GLfloat quat[] = {0.0f,0.0f,0.0f,0.0f};
//    mat4 glassS = scale(identity_mat4(),vec3(200.0f,140.0f,0) );
//    mat4 glassT = translate(identity_mat4(), vec3(100.0f,glass->glassHeight,55.0f));
//    mat4 glassR;
//    create_versor(quat, 90, -1.0f, 0.0f, 0.0f);
//    quat_to_mat4(glassR.m, quat);
    glass->modelMatrix = identity_mat4();
    glUniformMatrix4fv(glass->location_modelMatrix, 1, GL_FALSE, glass->modelMatrix.m);

    int length = 4;
    vec2 *dots = (vec2 *) malloc(sizeof(vec2) * length);

    for (int i = 0; i < length; i++) {

        if (i == 0) {
            dots[i] = vec2(0.1f, 0.1f);
        }
        if (i == 1) {
            dots[i] = vec2(0.9f, 0.1f);
        }
        if (i == 2) {
            dots[i] = vec2(0.1f, 0.9f);
        }
        if (i == 3) {
            dots[i] = vec2(0.9f, 0.9f);
        }

//        dots[i] = vec2((float) (0.2 * i), (float) (0.2 * i));
//        glUniformMatrix4fv(animal->bone_matrices_location[j], 1, GL_FALSE, identity_mat4().m);
    }
    glUniform2fv(glass->location_dots[0], length, dots->v);


}

void glassLoadTexture(Glass* glass, const char* name, int type){

    GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    unsigned char* image_data;
    int x ,y;
    loadImageFile(name, true, &image_data, &x,&y);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    free(image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if(type == GLASS_NORMAL )glass->normalMapTexture = texID;
    else if(type == GLASS_DUDV)glass->dudvTexture = texID;

}

void glassCreateVao(Glass* glass){

    GLfloat texcoords[] = {
            1.0f,1.0f,
            0.0f,1.0f,
            0.0f,0.0f,
            0.0f,0.0f,
            1.0f,0.0f,
            1.0f,1.0f,
    };

    GLfloat reflection_points[] = {

            /*1.75f,  1.0f, -1.5f,
            -1.75f,  1.0f, -1.5f,
            -1.75f, -1.0f, -1.5f,
            -1.75f, -1.0f, -1.5f,
            1.75f, -1.0f, -1.5f,
            1.75f,  1.0f, -1.5f,*/

            1.0f,  1.0f, -2.0f,
            -1.0f,  1.0f, -2.0f,
            -1.0f, -1.0f, -2.0f,
            -1.0f, -1.0f, -2.0f,
            1.0f, -1.0f, -2.0f,
            1.0f,  1.0f, -2.0f,

    };

    GLuint reflectionVbo = 0;
    glGenBuffers(1, &reflectionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, reflectionVbo);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), reflection_points, GL_STATIC_DRAW);
    glass->positionVbo = reflectionVbo;

    GLuint water_coords_vbo;
    glGenBuffers(1, &water_coords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, water_coords_vbo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
    glass->texCoordVbo = water_coords_vbo;

    GLuint waterReflectionVao = 0;
    glGenVertexArrays(1, &waterReflectionVao);
    glBindVertexArray(waterReflectionVao);
    glBindBuffer(GL_ARRAY_BUFFER, reflectionVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, water_coords_vbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glass->vao = waterReflectionVao;
}

GLuint glassCreateFrameBuffer(){

    GLuint frameBufferID;
    glGenFramebuffers(1,&frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    return frameBufferID;
}

GLuint glassCreateTextureAttachment(int width, int height){

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, textureID,0);
    return textureID;
}

GLuint glassCreateDepthTextureAttachment(int width, int height){

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT, textureID,0);
    return textureID;
}

GLuint glassCreateDepthBufferAttachment(int width, int height){

    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, depthBuffer);
    return depthBuffer;
}

void glassUnbindCurrentFrameBuffer(Window * hardware) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, hardware->vmode->width, hardware->vmode->height);
}

//call this function to tell opengl to render to our framebuffer object
void glassBindFrameBufer(GLuint frameBuffer, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
}

void glassGetUniforms(Glass* glass) {
    glass->location_reflectionTexture    = glGetUniformLocation(glass->shader, "reflectionTexture");
//    glass->location_refractionTexture    = glGetUniformLocation(glass->shader, "refractionTexture");
//    glass->location_dudv                 = glGetUniformLocation(glass->shader, "dudvMap");
    glass->location_viewMatrix           = glGetUniformLocation(glass->shader, "viewMatrix");
    glass->location_projMatrix           = glGetUniformLocation(glass->shader, "projectionMatrix");
    glass->location_modelMatrix          = glGetUniformLocation(glass->shader, "modelMatrix");
//    glass->location_moveFactor           = glGetUniformLocation(glass->shader, "moveFactor");
//    glass->location_cameraPosition       = glGetUniformLocation(glass->shader, "cameraPosition");
//    glass->location_normalMap            = glGetUniformLocation(glass->shader, "normalMap");
//    glass->location_lightColour          = glGetUniformLocation(glass->shader, "lightColour");
//    glass->location_lightPosition        = glGetUniformLocation(glass->shader, "lightPosition");
//    glass->location_depthMap             = glGetUniformLocation(glass->shader, "depthMap");

    char name[64];
    for (int j = 0; j < NUM_POINTS; j++) {
        sprintf(name, "dot[%i]", j);
        glass->location_dots[j] = glGetUniformLocation(glass->shader, name);
//        glUniformMatrix4fv(glass->bone_matrices_location[j], 1, GL_FALSE, identity_mat4().m);
    }
//    glass->location_dots = glGetUniformLocation(glass->shader, "dot");
}

void glassUpdate(Glass* glass){
//    glass->moveFactor += (WAVE_SPEED *  0.000003 * 500);
//    glass->moveFactor = fmod(glass->moveFactor, 1.0);
}

void glassRender(Glass* glass, Camera *camera){

    glUseProgram(glass->shader);
    glUniformMatrix4fv(glass->location_viewMatrix, 1, GL_FALSE, camera->viewMatrix.m);
    glBindVertexArray(glass->vao);
    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glEnableVertexAttribArray(2);
//    glEnableVertexAttribArray(3);
//    glEnableVertexAttribArray(4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glass->reflectionTexture);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, glass->refractionTexture);
//    glActiveTexture(GL_TEXTURE2);
//    glBindTexture(GL_TEXTURE_2D, glass->dudvTexture);
//    glActiveTexture(GL_TEXTURE3);
//    glBindTexture(GL_TEXTURE_2D, glass->normalMapTexture);
//    glActiveTexture(GL_TEXTURE4);
//    glBindTexture(GL_TEXTURE_2D, glass->refractionDepthTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
//    glDisableVertexAttribArray(1);
//    glDisableVertexAttribArray(2);
//    glDisableVertexAttribArray(3);
//    glDisableVertexAttribArray(4);
    glBindVertexArray(0);
}

void glassCleanUp(Glass* glass){

    glDeleteVertexArrays(1, &glass->vao);
    glDeleteBuffers(1, &glass->positionVbo);
    glDeleteBuffers(1, &glass->texCoordVbo);
    glDeleteFramebuffers(1, &glass->reflectionFrameBuffer);
    glDeleteTextures(1, &glass->reflectionTexture);
    glDeleteRenderbuffers(1, &glass->reflectionDepthBuffer);
    glDeleteFramebuffers(1, &glass->refractionFrameBuffer);
    glDeleteTextures(1, &glass->refractionTexture);
    glDeleteTextures(1, &glass->refractionDepthTexture);

}

