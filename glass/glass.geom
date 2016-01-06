#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 textureCoords[3];
in vec3 colours[3];

uniform float time;
out vec2 ftextureCoords;
out vec3 fcolours;

void main() {

  for(int i = 0; i < 3; i++) { // You used triangles, so it's always 3
      gl_Position = gl_in[i].gl_Position;
      ftextureCoords = textureCoords[i];
      fcolours = colours[i];
      EmitVertex();
  }
  EndPrimitive();

       /* gl_Position = gl_in[0].gl_Position + vec4(0.0,0.0,0.0,time*0.1);
          ftextureCoords = textureCoords[0];
          EmitVertex();

          gl_Position = gl_in[1].gl_Position + vec4(0.0,0.0,0.0,-time*0.1);
          ftextureCoords = textureCoords[1];
          EmitVertex();

          gl_Position = gl_in[2].gl_Position + vec4(0.0,0.0,0.0,0.0);
          ftextureCoords = textureCoords[2];
          EmitVertex();*/
}
