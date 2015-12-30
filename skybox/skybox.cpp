#include <utils/io/stb_image.h>
#include <malloc.h>
#include <utils/io/shader_loader.h>
#include <camera/camera.h>
#include <utils/math_utils/quat_funcs.h>
#include "skybox/skybox.h"

void skyInit(Skybox* sky, GLfloat* projection_matrix){

    skyLoadShaderProgram(sky);
    skyCreateVbo(sky);
    skyCreateVao(sky);
    glUseProgram(sky->shader);
    skyLoadCubeMapTextures(sky);
    skyGetUniforms(sky);
    glUniformMatrix4fv(sky->location_projection_mat, 1, GL_FALSE, projection_matrix);
}


void skyLoadCubeMapTextures(Skybox *sky){

    GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    for (int i = 0; i < 6; ++i) {
        const char* filename;
        switch (i) {
            case 0: filename = SKY_RIGHT; break;
            case 1: filename = SKY_LEFT;break;
            case 2: filename = SKY_TOP;break;
            case 3: filename = SKY_BOTTOM;break;
            case 4: filename = SKY_BACK;break;
            case 5: filename = SKY_FRONT;break;
            default:filename= 0;
        }

        if (filename == 0) {
            //make this function return error
            sky->texture = 0;
        }

        int x, y, n;
        int force_channels =4;
        unsigned char *image_data = stbi_load(filename , &x, &y, &n, force_channels);
        if (!image_data) {
            fprintf(stderr, "ERROR: could not load %s\n", filename );
        }
        if((x & (x-1)) != 0 || (y &(y - 1)) != 0) {
            fprintf(stderr, "WARNING:texture %s is not a power of 2 dimensiions\n", filename );
        }

        GLenum target = GL_ERROR_REGAL;
        switch (i) {
            case 0: target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;break;
            case 1: target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;break;
            case 2: target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;break;
            case 3: target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;break;
            case 4: target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;break;
            case 5: target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;break;
            default:break;
        }

        glTexImage2D(target, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        free(image_data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    sky->texture = texID;
}

void skyLoadShaderProgram(Skybox *sky){
    sky->shader = create_programme_from_files(SKY_VERTEX, SKY_FRAGMENT);
}

void skyCreateVbo(Skybox* sky){

    GLfloat SIZE = VALUE;
    GLfloat skybox_vertices[] = {
            -SIZE,  SIZE, -SIZE,
            -SIZE, -SIZE, -SIZE,
            SIZE, -SIZE, -SIZE,
            SIZE, -SIZE, -SIZE,
            SIZE,  SIZE, -SIZE,
            -SIZE,  SIZE, -SIZE,

            -SIZE, -SIZE,  SIZE,
            -SIZE, -SIZE, -SIZE,
            -SIZE,  SIZE, -SIZE,
            -SIZE,  SIZE, -SIZE,
            -SIZE,  SIZE,  SIZE,
            -SIZE, -SIZE,  SIZE,

            SIZE, -SIZE, -SIZE,
            SIZE, -SIZE,  SIZE,
            SIZE,  SIZE,  SIZE,
            SIZE,  SIZE,  SIZE,
            SIZE,  SIZE, -SIZE,
            SIZE, -SIZE, -SIZE,

            -SIZE, -SIZE,  SIZE,
            -SIZE,  SIZE,  SIZE,
            SIZE,  SIZE,  SIZE,
            SIZE,  SIZE,  SIZE,
            SIZE, -SIZE,  SIZE,
            -SIZE, -SIZE,  SIZE,

            -SIZE,  SIZE, -SIZE,
            SIZE,  SIZE, -SIZE,
            SIZE,  SIZE,  SIZE,
            SIZE,  SIZE,  SIZE,
            -SIZE,  SIZE,  SIZE,
            -SIZE,  SIZE, -SIZE,

            -SIZE, -SIZE, -SIZE,
            -SIZE, -SIZE,  SIZE,
            SIZE, -SIZE, -SIZE,
            SIZE, -SIZE, -SIZE,
            -SIZE, -SIZE,  SIZE,
            SIZE, -SIZE,  SIZE
    };

    glGenBuffers (1, &sky->vbo);
    glBindBuffer (GL_ARRAY_BUFFER, sky->vbo);
    glBufferData (GL_ARRAY_BUFFER, SKY_MAP_VERTEX_COUNT * 3 * sizeof(GLfloat), skybox_vertices, GL_STATIC_DRAW);
}

void skyCreateVao(Skybox* sky){

    glGenVertexArrays (1, &sky->vao);
    glBindVertexArray (sky->vao);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, sky->vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
}

void skyGetUniforms(Skybox* sky){
    sky->location_projection_mat = glGetUniformLocation(sky->shader, "projectionMatrix");
    sky->location_view_mat = glGetUniformLocation(sky->shader, "viewMatrix");
    sky->location_model_mat = glGetUniformLocation(sky->shader, "modelMatrix");
}

void skyUpdate(Skybox *sky){

    //calculate new sky ngle
    GLfloat quat[] = {0.0f,0.0f,0.0f,0.0f};
    sky->angle += 0.002f;
    if (sky->angle> 359) sky->angle= 0;
    create_versor(quat, sky->angle, 0.0f, 1.0f, 0.0f);
    quat_to_mat4(sky->modelMatrix.m, quat);
}

void skyRender(Skybox *sky, Camera* camera){

    float x = camera->viewMatrix.m[12];
    float y = camera->viewMatrix.m[13];
    float z = camera->viewMatrix.m[14];

    //The sky should move along with the camera, so never translate it
    camera->viewMatrix.m[12] = 0;
    camera->viewMatrix.m[13] = 0;
    camera->viewMatrix.m[14] = 0;

    glUseProgram(sky->shader);
    glUniformMatrix4fv(sky->location_model_mat, 1, GL_FALSE, sky->modelMatrix.m);
    glUniformMatrix4fv(sky->location_view_mat, 1, GL_FALSE, camera->viewMatrix.m);
    glBindVertexArray(sky->vao);
    glEnableVertexAttribArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sky->texture);
    glDrawArrays(GL_TRIANGLES, 0, SKY_MAP_VERTEX_COUNT);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    camera->viewMatrix.m[12] = x;
    camera->viewMatrix.m[13] = y;
    camera->viewMatrix.m[14] = z;

}

void skyCleanUp(Skybox* sky){

    glDeleteVertexArrays(1, &sky->vao);
    glDeleteBuffers(1, &sky->vbo);
}