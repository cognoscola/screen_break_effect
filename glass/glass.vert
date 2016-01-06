#version 410

layout(location = 0)in vec3 positions;
layout(location = 1)in vec2 texCoords;
layout(location = 2)in int triangle_id;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix[100];
uniform mat4 projectionMatrix;

out vec2 textureCoords;
out vec3 colours;

void main(void){

    colours = vec3(triangle_id/100.0, triangle_id/100.0, triangle_id/100.0);
    gl_Position = projectionMatrix * modelMatrix[triangle_id] *  vec4(positions,1.0);
    textureCoords = texCoords;

}