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

void glassInit(Glass *glass, Window *window, GLfloat* proj_mat) {

    //create texture objects for glass effects
    glassCreateVao(glass);
    glassCreateShardTransformations(glass);
    //create frame buffer stuff
    glass->framebuffer = glassCreateFrameBuffer();
    glass->framebufferTexture = glassCreateTextureAttachment(GLASS_REFLECTION_WIDTH, GLASS_REFLECTION_HEIGHT);
    glass->depthBuffer = glassCreateDepthBufferAttachment(GLASS_REFLECTION_WIDTH, GLASS_REFLECTION_HEIGHT);
    glassUnbindCurrentFrameBuffer(window);

    //create shader
    glass->shader = create_programme_from_files(GLASS_VERTEX, GLASS_FRAGMENT, GLASS_GEOMETRY);
    glUseProgram(glass->shader);
    glassGetUniforms(glass);

    //set initial shader settings
    glUniformMatrix4fv(glass->location_projMatrix, 1, GL_FALSE, proj_mat);
    glUniform1i(glass->location_frameTexture, 0 );
//    glass->modelMatrix = identity_mat4();
//    glUniformMatrix4fv(glass->location_modelMatrix, 1, GL_FALSE, glass->modelMatrix.m);

//    glUniform2fv(glass->location_dots[0], length, dots->v);
    glUseProgram(glass->debugShader);
    glass->location_projMattrixTest = glGetUniformLocation(glass->debugShader, "projectionMatrixT");
    glUniformMatrix4fv(glass->location_projMattrixTest, 1, GL_FALSE, proj_mat);

    glUseProgram(glass->screenShader);
    glass->location_screen_proj_matrix    = glGetUniformLocation(glass->screenShader, "projectionMatrix");
    glass->location_alpha = glGetUniformLocation(glass->screenShader, "alpha");
    glass->location_colour = glGetUniformLocation(glass->screenShader, "colour");
    glUniformMatrix4fv(glass->location_screen_proj_matrix, 1, GL_FALSE, proj_mat);
}

void glassCreateVao(Glass* glass){

    glass->debugShader = create_programme_from_files(vertex, fragment, trigeometry);

    //allocate memory to store our random points
    del_point2d_t* delPoints = (del_point2d_t *) malloc(sizeof(del_point2d_t) * POINTS);

    //place points at the corners of the quad
    delPoints[0].x = -1.0f;
    delPoints[0].y = -1.0f;
    delPoints[1].x = 1.0f;
    delPoints[1].y = -1.0f;
    delPoints[2].x = -1.0f;
    delPoints[2].y = 1.0f;
    delPoints[3].x = 1.0f;
    delPoints[3].y = 1.0f;

    for (int i = 4; i < POINTS; i++) {

        //place points on the edges of the quad
        if (i < 8) {
            delPoints[i].x = (float) (2.0f * (double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;
            delPoints[i].y = -1.0f;
        } else if (i < 12) {
            delPoints[i].x = (float) (2.0f * (double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;
            delPoints[i].y = 1.0f;
        } else if (i < 16) {
            delPoints[i].x = 1.0f;
            delPoints[i].y = (float) (2.0f * (double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;
        } else if (i < 20) {
            delPoints[i].x = -1.0f;
            delPoints[i].y =(float) (2.0f * (double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;
        }
        //in order to disperse points more evenly across the entire quad, the quad is split into 4 quadrants
        // and random points are generated within each quadrant

        else if (i < 30) {
            delPoints[i].x = ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) ;
            delPoints[i].y = ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) ;
        }else if (i < 40) {
            delPoints[i].x = ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) -1.0f ;
            delPoints[i].y = ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) ;
        }else if (i < 50){
            delPoints[i].x =  (float) ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;
            delPoints[i].y =  (float) ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 1.0f;
        }else if (i < 60){
            delPoints[i].x = (float) ((double) rand() / (double) ((unsigned) RAND_MAX + 1));
            delPoints[i].y = (float) ((double) rand() / (double) ((unsigned) RAND_MAX + 1)) -1.0f;
        }
        srand(i);
    }


    //perform delaunay triangulation of the points
    delaunay2d_t *delObject= delaunay2d_from(delPoints, POINTS);
    tri_delaunay2d_t* triangles = tri_delaunay2d_from(delObject);
    del_point2d_t ptZero;

    //fetch the number of points and triangles generated
    glass->num_points = triangles->num_points;
    glass->num_triangles = triangles->num_triangles;

    printf("Mesh has %d points\n", triangles->num_points);
    printf("Mesh has %d triangles\n", triangles->num_triangles);

    //allocate memory to store as vbos
    GLfloat* points = (GLfloat *) malloc(sizeof(GLfloat) * glass->num_points * glass->num_triangles     * DIMENSIONS);
    GLfloat* quadCoords = (GLfloat *) malloc(sizeof(GLfloat) * glass->num_points * glass->num_triangles * DIMENSIONS);
    GLfloat* texCoords = (GLfloat *) malloc(sizeof(GLfloat) * glass->num_points * glass->num_triangles  * 2 );
    GLint* triangleIds = (int*) malloc(sizeof(int) * glass->num_triangles * glass->num_points);
    glass->modelMats = (mat4 *) malloc(sizeof(mat4) * glass->num_triangles);

    for (int j = 0; j < triangles->num_triangles; j++) {
        for (int i = 0; i < 3; i++) {

            triangleIds[j * 3 + i] = j;
            int p0 = triangles->tris[j * 3 + i];
            ptZero = triangles->points[p0];

            points[(j * 3 + i) * DIMENSIONS + 0] = (GLfloat) ptZero.x;
            points[(j * 3 + i) * DIMENSIONS + 1] = (GLfloat) ptZero.y;
            points[(j * 3 + i) * DIMENSIONS + 2] = -1.999f;

            quadCoords[(j * 3 + i) * DIMENSIONS + 0] = (GLfloat) ptZero.x;
            quadCoords[(j * 3 + i) * DIMENSIONS + 1] = (GLfloat) ptZero.y;
//            quadCoords[(j * 3 + i) * DIMENSIONS + 2] = -4.5f;
            quadCoords[(j * 3 + i) * DIMENSIONS + 2] = -1.5f;

            texCoords[(j * 3 + i) * 2 + 0] = (GLfloat) ((ptZero.x + 1.0f) / 2.0f);
            texCoords[(j * 3 + i) * 2 + 1] = (GLfloat) ((ptZero.y + 1.0f) / 2.0f);

//            printf("Triangle[%i],Point:[%i] has Index:%i, &[%i],X:%f,Y:%f\n",j,i,p0,j*3 + i, ptZero.x, ptZero.y);
//            printf("Tex Coord: X:%f,Y:%f\n",texCoords[(j*3 + i) * 2 + 0], texCoords[(j*3 + i) * 2 + 0]);
        }
    }

    glUseProgram(glass->debugShader);
    glGenBuffers(1, &glass->debugVbo);
    glBindBuffer(GL_ARRAY_BUFFER, glass->debugVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * glass->num_points * glass->num_triangles * DIMENSIONS, points, GL_STATIC_DRAW);

    glGenVertexArrays(1, &glass->debugVao);
    glBindVertexArray(glass->debugVao);
    glBindBuffer(GL_ARRAY_BUFFER, glass->debugVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glUseProgram(glass->shader);
    GLuint reflectionVbo = 0;
    glGenBuffers(1, &reflectionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, reflectionVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * glass->num_points * glass->num_triangles * DIMENSIONS, quadCoords, GL_STATIC_DRAW);
    glass->positionVbo = reflectionVbo;

    GLuint water_coords_vbo;
    glGenBuffers(1, &water_coords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, water_coords_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* glass->num_points * glass->num_triangles * 2, texCoords, GL_STATIC_DRAW);
    glass->texCoordVbo = water_coords_vbo;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * glass->num_triangles * glass->num_points, triangleIds, GL_STATIC_DRAW);
    glass->triangleIdVbo = vbo;

    GLuint waterReflectionVao = 0;
    glGenVertexArrays(1, &waterReflectionVao);
    glBindVertexArray(waterReflectionVao);
    glBindBuffer(GL_ARRAY_BUFFER, reflectionVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, water_coords_vbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribIPointer(2, 1, GL_INT, 0, NULL);
    glEnableVertexAttribArray(2);
    glass->vao = waterReflectionVao;

    free(quadCoords);
    free(texCoords);
    free(points);
    free(triangleIds);
    delaunay2d_release(delObject);
    tri_delaunay2d_release(triangles);

    //create vao/vbo for the screens
    GLfloat screenCoords[] = {
            1.75f,1.0f,-1.4f,
            -1.75f,1.0f,-1.4f,
            -1.75f,-1.0f,-1.4f,
            -1.75f,-1.0f,-1.4f,
            1.75f,-1.0f,-1.4f,
            1.75f,1.0f,-1.4f
    };

    glass->screenShader = create_programme_from_files(SCREEN_VERTEX, SCREEN_FRAGMENT);
    glUseProgram(glass->screenShader);
    glGenBuffers(1, &glass->screenPositionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, glass->screenPositionVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3* 6, screenCoords, GL_STATIC_DRAW);

    glGenVertexArrays(1, &glass->screenVao);
    glBindVertexArray(glass->screenVao);
    glBindBuffer(GL_ARRAY_BUFFER, glass->screenPositionVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    printf("Done Init\n");
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
    glass->location_frameTexture = glGetUniformLocation(glass->shader, "reflectionTexture");
    glass->location_viewMatrix           = glGetUniformLocation(glass->shader, "viewMatrix");
    glass->location_projMatrix           = glGetUniformLocation(glass->shader, "projectionMatrix");

    char name[64];
    for (int k = 0; k < glass->num_triangles; k++) {
        sprintf(name, "modelMatrix[%i]",k);
        glass->location_model_matrices[k] = glGetUniformLocation(glass->shader, name);
        glUniformMatrix4fv(glass->location_model_matrices[k], 1, GL_FALSE, scale(identity_mat4(), vec3(1.78f,1.0f,1.0f)).m);
    }
}

void glassCreateShardTransformations(Glass* glass){
    //allocate memory to store transformation objects
    glass->transformations = (GlassTransformation*) malloc(sizeof(GlassTransformation) * glass->num_triangles);

    //for each triangle, generate some keyframes
    for (int l = 0; l < glass->num_triangles; l++) {

        GlassTransformation transformation;
        transformation.animationDuration = glass->num_triangles  * 0.2 + 0.4;

        //set the translations
        transformation.numPosKeys = 2 + glass->num_triangles;
        transformation.posKeys = (vec3 *) malloc(sizeof(vec3) * transformation.numPosKeys);
        transformation.posKeyTimes = (double *) malloc(sizeof(double) * transformation.numPosKeys);

        for (int i = 0; i < transformation.numPosKeys; i++) {
            //for the first 2 key frames, keep the pieces in place
            if(i < 2 ){
                transformation.posKeys[i] = vec3(0.0, 0.0f, 0.0f);
                transformation.posKeyTimes[i] = i * 0.25f;
            }
            //but aftwards, make them fly out of the screen on the right
            if(i > 1 ) {
                if ( (i -2 ) > (glass->num_triangles - l)/5 ) {
                    transformation.posKeys[i] = vec3(3.9f, 0.0f, 0.0f);
                    transformation.posKeyTimes[i] = i * 0.036f * (0.01f * (l + 310));
                }else{
                    transformation.posKeys[i] = vec3(0.0f, 0.0f, 0.0f);
                    transformation.posKeyTimes[i] = i * 0.10f;
                }
            }
        }

        //set the rotations
        transformation.numRotKeys = 2 + glass->num_triangles;
        transformation.rotKeys = (versor*) malloc(sizeof(versor) * transformation.numRotKeys);
        transformation.rotKeyTimes = (double *) malloc(sizeof(double) * transformation.numRotKeys);
        GLfloat quat[] = {0.0f,0.0f,0.0f,0.0f};

        //make the pieces turn randomly
        float randomTurnValue = (float) (8.0f * (double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 4.0f;
        float randomTurnValueY = (float) (90.0f * (double) rand() / (double) ((unsigned) RAND_MAX + 1)) - 45.0f;
        for (int j = 0; j < transformation.numRotKeys; j++) {
            //for first 2 key frames, rotate objects randomly in place

            if(j == 0){
                create_versor(quat, 0, 0.0f,0.0f,1.0f);
                transformation.rotKeyTimes[j] = j * 0.25f;
            }
            if(j == 1){
                create_versor(quat, randomTurnValue, 0.0f,0.0f,1.0f);
                transformation.rotKeyTimes[j] = j * 0.25f;
            }
            //later, rotate them as they fly out of the screen
            if(j >1){
                if ( (j - 2 ) > (glass->num_triangles - l)/5 ) {
                    create_versor(quat, randomTurnValueY , 1.0f,0.0f,0.0f);
                    transformation.rotKeyTimes[j] = j * 0.035f * (0.01f * (l + 310));

                }else{
                    create_versor(quat, randomTurnValue, 0.0f,0.0f,1.0f);
                    transformation.rotKeyTimes[j] = j * 0.10f;
                }
            }

            transformation.rotKeys[j].q[0] =quat[0];
            transformation.rotKeys[j].q[1] =quat[1];
            transformation.rotKeys[j].q[2] =quat[2];
            transformation.rotKeys[j].q[3] =quat[3];
        }
        srand(l);
        glass->transformations[l] = transformation;
    }
    //get shader location of our modelMatrices
}

void glassRender(Glass* glass, Camera *camera, double elapsedSeconds){

    //similar to skeleton animations, where we interpolate keyframes for each node
    //we interpolate keyframes for each triangle.
    //simply check the time, and then interpolate between two key states.

    glass->transitionTime += elapsedSeconds;
    for (int j = 0; j < glass->num_triangles; j++) {

        GlassTransformation transformation = glass->transformations[j];
        if (glass->transitionTime >= transformation.animationDuration) {
            glass->transitionTime = transformation.animationDuration;
        }
        //perform transformation
        mat4 nodeT = identity_mat4();
        if (transformation.numPosKeys > 0) {
            int prevKeys =0;
            int nextKeys =0;
            for (int i = 0; i < transformation.numPosKeys - 1; i++) {
                prevKeys = i;
                nextKeys =i +1;
                if (transformation.posKeyTimes[nextKeys] >= glass->transitionTime) {
                    break;
                }
            }
            float total_t = (float)(transformation.posKeyTimes[nextKeys] - transformation.posKeyTimes[prevKeys]);
            float t = (float)((glass->transitionTime - transformation.posKeyTimes[prevKeys]) / total_t);
            vec3 vi = transformation.posKeys[prevKeys];
            vec3 vf = transformation.posKeys[nextKeys];
            vec3 lerped = vi* (1.0f -t ) + vf* t;

            nodeT = translate(identity_mat4(), lerped);
        }

        //sphere interpolate rotations
        mat4 nodeR = identity_mat4();
        if(transformation.numRotKeys > 0) {
            int prevKeys = 0;
            int nextKeys = 0;
            for (int i = 0; i < transformation.numRotKeys - 1; i++) {
                prevKeys = i;
                nextKeys = i + 1;
                if (transformation.rotKeyTimes[nextKeys] >= glass->transitionTime) {
                    break;
                }
            }
            float total_t = (float)(transformation.rotKeyTimes[nextKeys] - transformation.rotKeyTimes[prevKeys]);
            float t = (float)((glass->transitionTime - transformation.rotKeyTimes[prevKeys]) / total_t);
            versor qi = transformation.rotKeys[prevKeys];
            versor qf = transformation.rotKeys[nextKeys];
            versor slerped = slerp(qi, qf, t);
            nodeR = quat_to_mat4(slerped);
        }
        glass->modelMats[j] = nodeT* nodeR * scale(identity_mat4(), vec3(1.78f,1.0f,1.0f));
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(glass->screenShader);
    glUniform1f(glass->location_colour, 0.0);
    if (glass->transitionTime < 1.6) {
        glUniform1f(glass->location_alpha, 1.0);
    }else{
        glUniform1f(glass->location_alpha, (GLfloat)(1- (glass->transitionTime-1.6f)));
    }
    glBindVertexArray(glass->screenVao);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    glUseProgram(glass->shader);
    glUniformMatrix4fv(glass->location_model_matrices[0], glass->num_triangles, GL_FALSE, glass->modelMats[0].m);
    glUniformMatrix4fv(glass->location_viewMatrix, 1, GL_FALSE, camera->viewMatrix.m);
    glBindVertexArray(glass->vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glass->framebufferTexture);
    glDrawArrays(GL_TRIANGLES, 0, glass->num_points * glass->num_triangles);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glUseProgram(glass->screenShader);
    glUniform1f(glass->location_alpha, (GLfloat)(1-(glass->transitionTime)));
    glUniform1f(glass->location_colour, 1.0);
    glBindVertexArray(glass->screenVao);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    //new stuff
/*    glUseProgram(glass->debugShader);
    glBindVertexArray(glass->debugVao);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, glass->num_points * glass->num_triangles);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    */
}

void glassCleanUp(Glass* glass){

    glDeleteVertexArrays(1, &glass->vao);
    glDeleteBuffers(1, &glass->positionVbo);
    glDeleteBuffers(1, &glass->texCoordVbo);
    glDeleteBuffers(1, &glass->triangleIdVbo);
    glDeleteFramebuffers(1, &glass->framebuffer);
    glDeleteTextures(1, &glass->framebufferTexture);
    glDeleteRenderbuffers(1, &glass->depthBuffer);

    glDeleteProgram(glass->debugShader);
    glDeleteProgram(glass->shader);
    glDeleteVertexArrays(1, &glass->debugVao);
    glDeleteBuffers(1, &glass->debugVbo);

    free(glass->transformations);
    free(glass->modelMats);
}

