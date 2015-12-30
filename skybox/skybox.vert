#version 410

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout(location = 0)in vec3 positions;
out vec3 textureCoords;

void main () {


   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4 (positions, 1.0);;
   textureCoords= positions;

}