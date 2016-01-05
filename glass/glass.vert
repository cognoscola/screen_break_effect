#version 410

layout(location = 0)in vec3 positions;
layout(location = 1)in vec2 texCoords;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

out vec2 textureCoords;

void main(void){
    gl_Position = projectionMatrix * modelMatrix *  vec4(positions,1.0);
    textureCoords = texCoords;
}