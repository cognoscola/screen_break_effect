//
// Created by alvaregd on 06/12/15.
//
#include <math.h>
#include <utils/io/texture.h>
#include <malloc.h>
#include <GL/glew.h>
#include <platform/glfw_launcher.h>
#include <utils/io/shader_loader.h>
#include <utils/math_utils/quat_funcs.h>
#include <camera/camera.h>
#include "water.h"

void waterInit(Water *water, Window *hardware, GLfloat* proj_mat) {

    //create texture objects for water effects
    waterLoadTexture(water, DUDV_FILE, DUDV);
    waterLoadTexture(water, NORMALMAP_FILE, NORMAL);
    waterCreateVao(water);

    //create frame buffer stuff
    water->reflectionFrameBuffer = createFrameBuffer();
    water->reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    water->reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    unbindCurrentFrameBuffer(hardware);
    water->refractionFrameBuffer = createFrameBuffer();
    water->refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    water->refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    unbindCurrentFrameBuffer(hardware);

    //create shader
    water->shader = create_programme_from_files(WATER_VERTEX, WATER_FRAGMENT);
    glUseProgram(water->shader);
    waterGetUniforms(water);

    //set initial shader settings
    glUniformMatrix4fv(water->location_projMatrix, 1, GL_FALSE, proj_mat);
    glUniform3f(water->location_lightColour , 1.0f,1.0f,1.0f);
    glUniform3f(water->location_lightPosition , 50.0f,100.0f,50.0f);
    glUniform1i(water->location_reflectionTexture,0 );
    glUniform1i(water->location_refractionTexture,1 );
    glUniform1i(water->location_dudv,2 );
    glUniform1i(water->location_normalMap,3);
    glUniform1i(water->location_depthMap,4);

    //calculate initial water position/orientation
    water->waterHeight = 5.0f;
    GLfloat quat[] = {0.0f,0.0f,0.0f,0.0f};
    mat4 waterS = scale(identity_mat4(),vec3(200.0f,140.0f,0) );
    mat4 waterT = translate(identity_mat4(), vec3(100.0f,water->waterHeight,55.0f));
    mat4 waterR;
    create_versor(quat, 90, -1.0f, 0.0f, 0.0f);
    quat_to_mat4(waterR.m, quat);
    water->modelMatrix = waterT * waterR * waterS;
    glUniformMatrix4fv(water->location_modelMatrix, 1, GL_FALSE, water->modelMatrix.m);

}


void waterLoadTexture(Water* water, const char* name, int type){

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

    if(type == NORMAL )water->normalMapTexture = texID;
    else if(type == DUDV)water->dudvTexture = texID;
}

void waterCreateVao(Water* water){

    GLfloat texcoords[] = {
            1.0f,1.0f,
            0.0f,1.0f,
            0.0f,0.0f,
            0.0f,0.0f,
            1.0f, 0.0f,
            1.0f,1.0f,
    };

    GLfloat reflection_points[] = {
            -0.75f, 0.25f,  0.0f,
            -0.25f, 0.25f,  0.0f,
            -0.25f, 0.75f,  0.0f,
            -0.25f, 0.75f,  0.0f,
            -0.75f, 0.75f,  0.0f,
            -0.75f, 0.25f,  0.0f
    };

    GLuint reflectionVbo = 0;
    glGenBuffers(1, &reflectionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, reflectionVbo);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), reflection_points, GL_STATIC_DRAW);
    water->positionVbo = reflectionVbo;

    GLuint water_coords_vbo;
    glGenBuffers(1, &water_coords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, water_coords_vbo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
    water->texCoordVbo = water_coords_vbo;

    GLuint waterReflectionVao = 0;
    glGenVertexArrays(1, &waterReflectionVao);
    glBindVertexArray(waterReflectionVao);
    glBindBuffer(GL_ARRAY_BUFFER, reflectionVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, water_coords_vbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    water->vao = waterReflectionVao;

}

GLuint createFrameBuffer(){

    GLuint frameBufferID;
    glGenFramebuffers(1,&frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    return frameBufferID;
}

GLuint createTextureAttachment(int width, int height){

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, textureID,0);
    return textureID;
}


GLuint createDepthTextureAttachment(int width, int height){

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT, textureID,0);
    return textureID;
}

GLuint createDepthBufferAttachment(int width, int height){

    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, depthBuffer);
    return depthBuffer;
}

void unbindCurrentFrameBuffer(Window * hardware) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, hardware->vmode->width, hardware->vmode->height);
}

//call this function to tell opengl to render to our framebuffer object
void bindFrameBufer(GLuint frameBuffer, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
}

void waterGetUniforms(Water* water) {
    water->location_reflectionTexture    = glGetUniformLocation(water->shader, "reflectionTexture");
    water->location_refractionTexture    = glGetUniformLocation(water->shader, "refractionTexture");
    water->location_dudv                 = glGetUniformLocation(water->shader, "dudvMap");
    water->location_viewMatrix           = glGetUniformLocation(water->shader, "viewMatrix");
    water->location_projMatrix           = glGetUniformLocation(water->shader, "projectionMatrix");
    water->location_modelMatrix          = glGetUniformLocation(water->shader, "modelMatrix");
    water->location_moveFactor           = glGetUniformLocation(water->shader, "moveFactor");
    water->location_cameraPosition       = glGetUniformLocation(water->shader, "cameraPosition");
    water->location_normalMap            = glGetUniformLocation(water->shader, "normalMap");
    water->location_lightColour          = glGetUniformLocation(water->shader, "lightColour");
    water->location_lightPosition        = glGetUniformLocation(water->shader, "lightPosition");
    water->location_depthMap             = glGetUniformLocation(water->shader, "depthMap");
}

void waterUpdate(Water* water){
    water->moveFactor += (WAVE_SPEED *  0.000003 * 500);
    water->moveFactor = fmod(water->moveFactor, 1.0);
}

void waterRender(Water* water, Camera *camera){

    glUseProgram(water->shader);
    glUniform1f(water->location_moveFactor,        (GLfloat)water->moveFactor);
    glUniform3f(water->location_cameraPosition,    camera->pos[0],camera->pos[1],camera->pos[2]);
    glUniformMatrix4fv(water->location_viewMatrix, 1, GL_FALSE, camera->viewMatrix.m);
    glBindVertexArray(water->vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, water->reflectionTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, water->refractionTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, water->dudvTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, water->normalMapTexture);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, water->refractionDepthTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glBindVertexArray(0);
}

void waterCleanUp(Water* water){

    glDeleteVertexArrays(1, &water->vao);
    glDeleteBuffers(1, &water->positionVbo);
    glDeleteBuffers(1, &water->texCoordVbo);
    glDeleteFramebuffers(1, &water->reflectionFrameBuffer);
    glDeleteTextures(1, &water->reflectionTexture);
    glDeleteRenderbuffers(1, &water->reflectionDepthBuffer);
    glDeleteFramebuffers(1, &water->refractionFrameBuffer);
    glDeleteTextures(1, &water->refractionTexture);
    glDeleteTextures(1, &water->refractionDepthTexture);

}

