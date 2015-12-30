//
// Created by alvaregd on 06/12/15.
//

#include <stdio.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <utils/io/texture.h>
#include <utils/io/shader_loader.h>
#include "mesh.h"

void meshInit(Mesh* mesh, GLfloat* proj_mat){
    assert(meshLoadMeshFile(MESH_FILE, &mesh->vao, &mesh->vertexCount));
    meshLoadTexture(mesh);
    meshLoadShaderProgram(mesh);
    glUseProgram(mesh->shader);
    meshGetUniforms(mesh);
    glUniform4f(mesh->location_clip_plane, 0.0f, -1.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(mesh->location_projection_mat , 1, GL_FALSE, proj_mat);
    mat4 s = scale(identity_mat4(), vec3(10,10,10));
    mesh->modelMatrix = s;
    glUniformMatrix4fv(mesh->location_model_mat , 1, GL_FALSE, mesh->modelMatrix.m);

}

bool meshLoadMeshFile(const char *fileName, GLuint *vao, int *point_count){

    const aiScene *scene = aiImportFile(fileName, aiProcess_Triangulate);
    if (!scene) {
        fprintf(stderr, "ERROR: reading mesh %s\n", fileName);
        return false;
    }

    printf(" %i animations\n", scene->mNumAnimations);
    printf(" %i Cameras\n", scene->mNumCameras);
    printf(" %i lights\n", scene->mNumLights);
    printf(" %i Materials\n", scene->mNumMaterials);
    printf(" %i Meshes\n", scene->mNumMeshes);
    printf(" %i textures\n", scene->mNumTextures);

    /*get the first mesh */
    const aiMesh* mesh = scene->mMeshes[0];
    printf("  %i vertices in mesh[0]\n", mesh->mNumVertices);

    /**pass back the mesh count */
    *point_count  = mesh->mNumVertices;

    /* generate */
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    /** make the data contiguous */
    GLfloat * points = NULL;
    GLfloat * normals = NULL;
    GLfloat * texcoords = NULL;

    if (mesh->HasPositions()) {
        points = (GLfloat *) malloc(*point_count * 3 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(mesh->mVertices[i]);
            points[i * 3 + 0] = (GLfloat)vp->x;
            points[i * 3 + 1] = (GLfloat)vp->y;
            points[i * 3 + 2] = (GLfloat)vp->z;
        }
    }

    if (mesh->HasNormals()) {
        normals = (GLfloat *) malloc(*point_count * 3 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(mesh->mNormals[i]);
            normals[i * 3 + 0] = (GLfloat)vp->x;
            normals[i * 3 + 1] = (GLfloat)vp->y;
            normals[i * 3 + 2] = (GLfloat)vp->z;

        }
    }

    if (mesh->HasTextureCoords(0)) {
        texcoords = (GLfloat *) malloc(*point_count * 2 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(mesh->mTextureCoords[0][i]);
            texcoords[i * 2 + 0] = (GLfloat)vp->x;
            texcoords[i * 2 + 1] = (GLfloat)vp->y;

        }
    }

    /** make vbos*/
    if (mesh->HasPositions()) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * *point_count * sizeof(GLfloat), points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
        free(points);
    }

    if (mesh->HasNormals()) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * *point_count * sizeof(GLfloat), normals, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
        free(normals);
    }

    if (mesh->HasTextureCoords(0)) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 2 * *point_count * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
        free(texcoords);
    }

    if (mesh->HasTangentsAndBitangents()) {

    }
    aiReleaseImport(scene);
    printf("Mesh loaded\n");
    return true;
}


void meshLoadTexture(Mesh* mesh){

    GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    unsigned char* image_data;
    int x ,y;
    loadImageFile(TERRAIN_TEXTURE, true, &image_data, &x,&y);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    free(image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    mesh->texture = texID;
}

void meshLoadShaderProgram(Mesh * mesh){

    mesh->shader = create_programme_from_files(MESH_VERTEX, MESH_FRAGMENT);
}

void meshGetUniforms(Mesh* mesh){
    mesh->location_model_mat   = glGetUniformLocation(mesh->shader, "modelMatrix");
    mesh->location_view_mat   = glGetUniformLocation(mesh->shader, "viewMatrix");
    mesh->location_projection_mat  = glGetUniformLocation(mesh->shader, "projectionMatrix");
    mesh->location_clip_plane      = glGetUniformLocation(mesh->shader, "plane");
}

void meshRender(Mesh* mesh, Camera* camera, GLfloat planeHeight){

    glUseProgram(mesh->shader);
    glUniform4f(mesh->location_clip_plane, 0.0f, 1.0f, 0.0f, planeHeight);
    glUniformMatrix4fv(mesh->location_view_mat, 1, GL_FALSE, camera->viewMatrix.m);
    glUniformMatrix4fv(mesh->location_model_mat, 1, GL_FALSE, mesh->modelMatrix.m);
    glBindVertexArray(mesh->vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh->texture);
    glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

}

void meshCleanUp(Mesh *mesh){
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);

}