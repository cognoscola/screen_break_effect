//
// Created by alvaregd on 30/12/15.
//
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
    glassCreateVao(glass);

    //create frame buffer stuff
    glass->reflectionFrameBuffer = glassCreateFrameBuffer();
    glass->reflectionTexture = glassCreateTextureAttachment(GLASS_REFLECTION_WIDTH, GLASS_REFLECTION_HEIGHT);
    glass->reflectionDepthBuffer = glassCreateDepthBufferAttachment(GLASS_REFLECTION_WIDTH, GLASS_REFLECTION_HEIGHT);
    glassUnbindCurrentFrameBuffer(hardware);


    //create shader
    glass->shader = create_programme_from_files(GLASS_VERTEX, GLASS_FRAGMENT);
    glUseProgram(glass->shader);
    glassGetUniforms(glass);

    //set initial shader settings
    glUniformMatrix4fv(glass->location_projMatrix, 1, GL_FALSE, proj_mat);
    glUniform1i(glass->location_reflectionTexture, 0 );
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

    }

    glUniform2fv(glass->location_dots[0], length, dots->v);

    //TODO new stuff here
    glass->sampleShader= create_programme_from_files(vertex, fragment,geometry);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    float points[] = {
            -0.45f,  0.45f,
            0.45f,  0.45f,
            0.45f, -0.45f,
            -0.45f, -0.45f,
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

// Create VAO

    glGenVertexArrays(1, &glass->sampleVao);
    glBindVertexArray(glass->sampleVao);

// Specify layout of point data
//    glass->location_pos = glGetAttribLocation(glass->sampleShader, "pos");
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);


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
    glass->location_viewMatrix           = glGetUniformLocation(glass->shader, "viewMatrix");
    glass->location_projMatrix           = glGetUniformLocation(glass->shader, "projectionMatrix");
    glass->location_modelMatrix          = glGetUniformLocation(glass->shader, "modelMatrix");

    char name[64];
    for (int j = 0; j < NUM_POINTS; j++) {
        sprintf(name, "dot[%i]", j);
        glass->location_dots[j] = glGetUniformLocation(glass->shader, name);
    }
}



void glassRender(Glass* glass, Camera *camera) {

  /*  glUseProgram(glass->shader);
    glUniformMatrix4fv(glass->location_viewMatrix, 1, GL_FALSE, camera->viewMatrix.m);
    glBindVertexArray(glass->vao);
    glEnableVertexAttribArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glass->reflectionTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);*/

    //new stuff
    glClearColor(0.0,0.0,0.0f,1.0f);
    glUseProgram(glass->sampleShader);
    glBindVertexArray(glass->sampleVao);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_POINTS, 0, 4);

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

