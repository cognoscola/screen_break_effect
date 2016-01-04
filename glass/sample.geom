#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

//in vec3 vColor[];
//in float vSides[];

//out vec3 fColor;
const float PI = 3.1415926;

void main() {

           gl_Position = gl_in[0].gl_Position + vec4(-0.01, 0.01, 0.0, 0.0);
           EmitVertex();

           gl_Position = gl_in[0].gl_Position + vec4(-0.01, -0.01, 0.0, 0.0);
           EmitVertex();

           gl_Position = gl_in[0].gl_Position + vec4(0.01, 0.01, 0.0, 0.0);
           EmitVertex();

           gl_Position = gl_in[0].gl_Position + vec4(0.01, -0.01, 0.0, 0.0);
           EmitVertex();

           EndPrimitive();

  /*  for (int i = 0; i <= 11; i++) {
            // Angle between each side in radians
            float ang = PI * 2.0 / 11 * i;
            // Offset from center of point (0.3 to accomodate for aspect ratio)
            vec4 offset = vec4(cos(ang) * 0.1, -sin(ang) * 0.1, 0.0, 0.0);
            gl_Position = gl_in[0].gl_Position + offset;
            EmitVertex();
    }
    EndPrimitive();*/
}