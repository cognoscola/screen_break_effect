//
// Created by alvaregd on 30/12/15.
//
#include <malloc.h>
#include <GL/glew.h>
#include <platform/glfw_launcher.h>
#include <utils/io/shader_loader.h>
#include <utils/math_utils/quat_funcs.h>
#include <camera/camera.h>
#include <libs/delaunay.h>
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
    glass->shader = create_programme_from_files(GLASS_VERTEX, GLASS_FRAGMENT, GLASS_GEOMETRY);
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

//    glUniform2fv(glass->location_dots[0], length, dots->v);
    glUseProgram(glass->sampleShader);
    glass->location_projMattrixTest = glGetUniformLocation(glass->sampleShader, "projectionMatrixT");
    glUniformMatrix4fv(glass->location_projMattrixTest, 1, GL_FALSE, proj_mat);
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

         /*   1.75f,  1.0f, -1.5f,
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

//TODO new stuff here
    glass->sampleShader= create_programme_from_files(vertex, fragment, trigeometry);
//    (float)(RAY_AREA_HEIGHT * (double) rand() / (double)((unsigned)RAND_MAX + 1) )

#define DIMENSIONS 3
#define POINTS 60


    del_point2d_t* delPoints = (del_point2d_t *) malloc(sizeof(del_point2d_t) * POINTS);

//    points[0 * DIMENSIONS + 0 ] = -1.0f;
//    points[0 * DIMENSIONS + 1 ] = -1.0f;
//    points[0 * DIMENSIONS + 2 ] = -1.999f;

    delPoints[0].x = -1.0f;
    delPoints[0].y = -1.0f;

//    points[1 * DIMENSIONS + 0 ] =  1.0f;
//    points[1 * DIMENSIONS + 1 ] = -1.0f;
//    points[1 * DIMENSIONS + 2 ] = -1.999f;

    delPoints[1].x = 1.0f;
    delPoints[1].y = -1.0f;

//    points[2 * DIMENSIONS + 0 ] = -1.0f;
//    points[2 * DIMENSIONS + 1 ] =  1.0f;
//    points[2 * DIMENSIONS + 2 ] = -1.999f;

    delPoints[2].x = -1.0f;
    delPoints[2].y = 1.0f;

//    points[3 * DIMENSIONS + 0 ] =  1.0f;
//    points[3 * DIMENSIONS + 1 ] =  1.0f;
//    points[3 * DIMENSIONS + 2 ] = -1.999f;

    delPoints[3].x = 1.0f;
    delPoints[3].y = 1.0f;

    double ptX;
    double ptY;

    for (int i = 4; i < POINTS; i++) {
        if (i < 8) {
            ptX = (float) (2.0 * (double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;
//            points[i * DIMENSIONS + 0] = ptX;
//            points[i * DIMENSIONS + 1] = -1.0f;
//            points[i * DIMENSIONS + 2] = -1.999f;
            delPoints[i].x = ptX;
            delPoints[i].y = -1.0f;

        } else if (i < 12) {
            ptX = (float) (2.0f * (double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;

//            points[i * DIMENSIONS + 0] = ptX;
//            points[i * DIMENSIONS + 1] =  1.0f;
//            points[i * DIMENSIONS + 2] = -1.999f;

            delPoints[i].x = ptX;
            delPoints[i].y = 1.0f;

        } else if (i < 16) {

            ptY = (float) (2.0f * (double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;

//            points[i * DIMENSIONS + 0] = 1.0f;
//            points[i * DIMENSIONS + 1] = ptY;
//            points[i * DIMENSIONS + 2] = -1.999f;

            delPoints[i].x = 1.0f;
            delPoints[i].y = ptY;

        } else if (i < 20) {

            ptY =(float) (2.0f * (double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;

//            points[i * DIMENSIONS + 0] = -1.0f;
//            points[i * DIMENSIONS + 1] = ptY;
//            points[i * DIMENSIONS + 2] = -1.999f;

            delPoints[i].x = -1.0f;
            delPoints[i].y = ptY;


        }else if (i < 30) {

            //spread 10 points randomly on Q1

            ptX =  ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) ;
            ptY =  ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) ;

//            points[i * DIMENSIONS + 0] = ptX;
//            points[i * DIMENSIONS + 1] = ptY;
//            points[i * DIMENSIONS + 2] = -1.999f;

            delPoints[i].x = ptX;
            delPoints[i].y = ptY;

        }else if (i < 40) {


            ptX = ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) -1.0f ;
            ptY = ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) ;

            //Q2
//            points[i * DIMENSIONS + 0] = ptX;
//            points[i * DIMENSIONS + 1] = ptY;
//            points[i * DIMENSIONS + 2] = -1.999f;


            delPoints[i].x = ptX;
            delPoints[i].y = ptY;

        }else if (i < 50){
            //Q3

            ptX = (float) ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;
            ptY = (float) ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;

            //Q2
//            points[i * DIMENSIONS + 0] = ptX;
//            points[i * DIMENSIONS + 1] = ptY;
//            points[i * DIMENSIONS + 2] = -1.999f;

            delPoints[i].x = ptX;
            delPoints[i].y = ptY;
        }else if (i < 60){
            //Q4
            ptX = (float) ((double) rand() / (double) ((unsigned) RAND_MAX + 1))  ;
            ptY = (float) ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) -1.0f;

            //Q2
//            points[i * DIMENSIONS + 0] = ptX;
//            points[i * DIMENSIONS + 1] = ptY;
//            points[i * DIMENSIONS + 2] = -1.999f;

            delPoints[i].x = ptX;
            delPoints[i].y = ptY;
        }
/*
        printf("Index:%i, X:%f, Y:%f, Z:%f\n", i, points[i * DIMENSIONS + 0], points[i * DIMENSIONS + 1],
               points[i * DIMENSIONS + 2]);*/
    }

    delaunay2d_t *delObject= delaunay2d_from(delPoints, POINTS);
    tri_delaunay2d_t* triangles = tri_delaunay2d_from(delObject);

 /*   del_point2d_t ptZero =  triangles->points[triangles->tris[0]];
    points[0 * DIMENSIONS + 0 ] = (GLfloat)ptZero.x;
    points[0 * DIMENSIONS + 1 ] = (GLfloat)ptZero.y;
    points[0 * DIMENSIONS + 2 ] = -1.999f;
    printf("Pt0 X:%f,Y:%f\n", ptZero.x, ptZero.y);

    ptZero =  triangles->points[triangles->tris[1]];
    points[1 * DIMENSIONS + 0 ] = (GLfloat)ptZero.x;
    points[1 * DIMENSIONS + 1 ] = (GLfloat)ptZero.y;
    points[1 * DIMENSIONS + 2 ] = -1.999f;
    printf("Pt1 X:%f,Y:%f\n", ptZero.x, ptZero.y);

    ptZero =  triangles->points[triangles->tris[2]];
    points[2 * DIMENSIONS + 0 ] = (GLfloat)ptZero.x;
    points[2 * DIMENSIONS + 1 ] = (GLfloat)ptZero.y;
    points[2 * DIMENSIONS + 2 ] = -1.999f;
    printf("Pt2 X:%f,Y:%f\n", ptZero.x, ptZero.y)*/;

    del_point2d_t ptZero;

    glass->num_points = triangles->num_points;
    glass->num_triangles = triangles->num_triangles;

    printf("Mesh has %d points\n", triangles->num_points);
    printf("Mesh has %d triangles\n", triangles->num_triangles);

    GLfloat *points = (GLfloat *) malloc(sizeof(GLfloat) * glass->num_points * glass->num_triangles * DIMENSIONS);

    for (int j = 0; j < triangles->num_triangles; j++) {
        for (int i = 0; i < 3; i++) {
            int p0 = triangles->tris[j * 3 + i];
            ptZero =  triangles->points[p0];
            points[(j*3 + i) * DIMENSIONS + 0] = (GLfloat)ptZero.y;
            points[(j*3 + i) * DIMENSIONS + 1] = (GLfloat)ptZero.x;
            points[(j*3 + i) * DIMENSIONS + 2] = -1.999f;
            printf("Point:%i has Index:%i X:%f,Y:%f\n",j*3 + i,triangles->tris[j], ptZero.x, ptZero.y);
        }
    }

    glGenBuffers(1, &glass->sampleVbo);
    glBindBuffer(GL_ARRAY_BUFFER, glass->sampleVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * glass->num_points * glass->num_triangles * DIMENSIONS, points, GL_STATIC_DRAW);

    free(points);
    delaunay2d_release(delObject);
    tri_delaunay2d_release(triangles);

    glGenVertexArrays(1, &glass->sampleVao);
    glBindVertexArray(glass->sampleVao);
    glBindBuffer(GL_ARRAY_BUFFER, glass->sampleVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

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
    glass->location_time                 = glGetUniformLocation(glass->shader, "time");
}

void glassRender(Glass* glass, Camera *camera, double time) {

    glUseProgram(glass->shader);
    glUniform1f(glass->location_time, (float) time);
    glUniformMatrix4fv(glass->location_viewMatrix, 1, GL_FALSE, camera->viewMatrix.m);
    glBindVertexArray(glass->vao);
    glEnableVertexAttribArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glass->reflectionTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    //new stuff
    glUseProgram(glass->sampleShader);
    glBindVertexArray(glass->sampleVao);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, glass->num_points * glass->num_triangles);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}

void glassCleanUp(Glass* glass){

    glDeleteVertexArrays(1, &glass->vao);
    glDeleteBuffers(1, &glass->positionVbo);
    glDeleteBuffers(1, &glass->texCoordVbo);
    glDeleteFramebuffers(1, &glass->reflectionFrameBuffer);
    glDeleteTextures(1, &glass->reflectionTexture);
    glDeleteRenderbuffers(1, &glass->reflectionDepthBuffer);

    glDeleteProgram(glass->sampleShader);
    glDeleteProgram(glass->shader);
    glDeleteVertexArrays(1, &glass->sampleVao);
    glDeleteBuffers(1, &glass->sampleVbo);
}

