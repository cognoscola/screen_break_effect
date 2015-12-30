#version 410

layout(location = 0)in vec3 positions;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

out vec2 textureCoords;
out vec4 clipSpace;
out vec3 toCameraVector;
out vec3 fromLightVector;

const float tiling = 6.0;

//Fesnel effect stuff
uniform vec3 cameraPosition;
uniform vec3 lightPosition; //

void main(void){

    vec4 worldPosition = viewMatrix * modelMatrix * vec4(positions,1.0);
    clipSpace = projectionMatrix * worldPosition;
    gl_Position = clipSpace;
    textureCoords = vec2(positions.x/2.0 + 0.5, positions.y/2.0 + 0.5);

    //fresnel stuff
    toCameraVector = cameraPosition - worldPosition.xyz;
    fromLightVector = worldPosition.xyz - lightPosition;

}