//
// Created by alvaregd on 02/12/15.
//
#include <math.h>
#include <platform/glfw_launcher.h>
#include "camera.h"

#define PI 3.14159265359
#define DEG_TO_RAD (2.0 * PI) / 360.0

void cameraInit(Camera * camera, Window *window){

    float near = 0.1f;
    float far = 300.0f;
    double fov = 67.0f * DEG_TO_RAD;
    float aspect = (float) window->vmode->width / (float) window->vmode->height;

    // matrix components
    double range = tan (fov * 0.5f) * near;
    double Sx = (2.0f * near) / (range * aspect + range * aspect);
    double Sy = near / range;
    float Sz = -(far + near) / (far - near);
    float Pz = -(2.0f * far * near) / (far - near);

    camera->proj_mat = new GLfloat[16];
    camera->proj_mat[0] = Sx;
    camera->proj_mat[1] = 0.0f;
    camera->proj_mat[2] = 0.0f;
    camera->proj_mat[3] = 0.0f;
    camera->proj_mat[4] = 0.0f;
    camera->proj_mat[5] = Sy;
    camera->proj_mat[6] = 0.0f;
    camera->proj_mat[7] = 0.0f;
    camera->proj_mat[8] = 0.0f;
    camera->proj_mat[9] = 0.0f;
    camera->proj_mat[10] = Sz;
    camera->proj_mat[11] = -1.0f;
    camera->proj_mat[12] = 0.0f;
    camera->proj_mat[13] = 0.0f;
    camera->proj_mat[14] = Pz;
    camera->proj_mat[15] = 0.0f;


    //create init view matrix
    camera->pos[0] = 0.0f; // don't start at zero, or we will be too close
    camera->pos[1] = 10.0f; // don't start at zero, or we will be too close
    camera->pos[2] = 0.5f; // don't start at zero, or we will be too close
    camera->T =translate (identity_mat4 (), vec3 (-camera->pos[0], -camera->pos[1], -camera->pos[2]));
    camera->Rpitch = rotate_y_deg (identity_mat4 (), -camera->yaw);
    camera->Ryaw = rotate_y_deg (identity_mat4 (), -camera->yaw);
    camera->viewMatrix = camera->Rpitch * camera->T;
}

/**
 * Calculate the player's kinematics and render it
 * When we detect a keypress, push becomes 1 (positive acceleration) until we release the key.
 * When key releases pushing becomes -1 (negative acceleration) to indicate that we are slowing down.
 */
void updateMovement(Camera* camera, Input* input) {

    //if any of the WASD keys are pressed, the camera will move
    if(input->wPressed || input->sPressed || input->aPressed || input->dPressed) {
        camera->pushing = 1;
    }

    //while we push,
    if (camera->pushing) {
        //set linear motion
        const double maxVelocity = 0.1 * (camera->pushing> 0);
        const double acceleration= camera->pushing>0 ? 0.2:0.1;

        if(camera->move_angle == 90.0f || camera->move_angle == -90.0f) {
            //Player has pressed either straf left or straf right, calculate the direction Vector using cross product
            //of actor's heading and the up direction
            vec3 left = cross(vec3(camera->viewMatrix.m[2],camera->viewMatrix.m[6],camera->viewMatrix.m[10]),
                              vec3(camera->viewMatrix.m[1],camera->viewMatrix.m[5],camera->viewMatrix.m[9]));

            //update the camera's velocity accordingly
            camera->velocity.v[0] =(float)(camera->velocity.v[0] * (1-acceleration) +
                                           ( left.v[0]) * ((camera->move_angle == 90 )? 1:-1) * (acceleration *maxVelocity));
            camera->velocity.v[2] =(float)(camera->velocity.v[2] * (1-acceleration) +
                                           ( left.v[2]) * ((camera->move_angle == 90 )? 1:-1) * (acceleration *maxVelocity));
        }else{
            //player has hit forward (w) or backwards (s). update the velocity in these directions
            camera->velocity.v[0] =(float)(camera->velocity.v[0] * (1-acceleration) +
                                           ( camera->viewMatrix.m[2]) * ((camera->move_angle == 180 )? -1:1) * (acceleration *maxVelocity));
            camera->velocity.v[1] =(float)(camera->velocity.v[1] * (1-acceleration) +
                                           ( camera->viewMatrix.m[6]) * ((camera->move_angle == 180 )? -1:1) * (acceleration *maxVelocity));
            camera->velocity.v[2] =(float)(camera->velocity.v[2] * (1-acceleration) +
                                           ( camera->viewMatrix.m[10]) * ((camera->move_angle == 180 )? -1:1) * (acceleration *maxVelocity));
        }
        camera->moving = true;
    }

    //while we are moving (velocity is nonzero), update the camera's position
    if (camera->moving) {
        camera->pos[0] += -camera->velocity.v[0] *0.3f;
        camera->pos[2] += -camera->velocity.v[2] *0.3f;
        camera->pos[1] += -camera->velocity.v[1] *0.3f;
        if(dot(camera->velocity,camera->velocity) < 1e-9) {
            camera->velocity.v[0] = camera->velocity.v[2] = camera->velocity.v[1] = 0.0f;
            camera->pushing = 0;
            camera->moving = false;
        }
    }
    if(camera->pushing){
        camera->pushing = -1;
    }
}

/**
 * calculate a new View Matrix
 */
void calculateViewMatrices(Camera *camera){
    camera->T = translate (identity_mat4 (), vec3 (-camera->pos[0], -camera->pos[1], -camera->pos[2]));
    camera->viewMatrix = camera->Rpitch * camera->Ryaw * camera->T;
}
