#version 410

layout(location = 0)in vec3 pos;
//layout(location = 1)in vec3 color;
//layout(location = 2)in float sides;

//out vec3 vColor;
//out float vSides;

uniform mat4 projectionMatrixT;

void main(void){
    gl_Position = projectionMatrixT * vec4(pos,1.0);
//    vColor = color;
//    vSides = sides;

}