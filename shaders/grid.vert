#version 410

uniform mat4 view, proj;
layout(location = 1) in vec3 vertex_colour;

in vec3 vertex_points;
out vec3 colour;

uniform vec4  plane = vec4(0,-1,0,-1);

void main () {

    colour = vertex_colour;
    vec4 vsPos =  proj *  view * vec4 (vertex_points, 1.0);
    gl_Position =  vsPos;

    gl_ClipDistance[0] = dot(plane,vsPos);
}