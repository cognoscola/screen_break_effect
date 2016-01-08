#version 410

layout(location = 0)in vec3 positions;
layout(location = 1)in vec2 texCoords;
layout(location = 2)in int triangle_id;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix[110];
uniform mat4 projectionMatrix;

const vec3 lightPosition = {0.0,0.0,5.0};

out vec2 textureCoords;
out vec3 colours;

out vec3 toLightVector;
out vec3 toCameraVector;

void main(void){
    colours = vec3(triangle_id/100.0, triangle_id/100.0, triangle_id/100.0);
    vec4 worldPosition = modelMatrix[triangle_id] *  vec4(positions,1.0);
    gl_Position = projectionMatrix * worldPosition;
    textureCoords = texCoords;

    toLightVector = lightPosition - worldPosition.xyz;
    toCameraVector = vec3(5.0,1.0,-2.0) - worldPosition.xyz;

}