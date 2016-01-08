#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 textureCoords[3];
in vec3 colours[3];
in vec3 toLightVector[3];
in vec3 toCameraVector[3];

out vec2 ftextureCoords;
out vec3 fcolours;
out vec3 fnormal;
out vec3 ftoLightVector;
out vec3 ftoCameraVector;

const float normalLength = 1.0;

void main() {

  for(int i = 0; i < 3; i++) { // You used triangles, so it's always 3
      gl_Position = gl_in[i].gl_Position;
      ftextureCoords = textureCoords[i];
      fcolours = colours[i];
      ftoLightVector = toLightVector[i];
      ftoCameraVector = toCameraVector[i];
      EmitVertex();
  }

  //get the normal
   vec3 P0 = gl_in[0].gl_Position.xyz;
   vec3 P1 = gl_in[1].gl_Position.xyz;
   vec3 P2 = gl_in[2].gl_Position.xyz;
   vec3 V0 = P0 - P1;
   vec3 V1 = P2 - P1;
   vec3 N = cross(V1,V0);
   fnormal= N;

   //center of triangle
  /* vec3 P = (P0+P1+P2)/3.0;

   gl_Position = vec4(P,1.0);
   fcolours = vec3(1.0,0.0,0.0);
   EmitVertex();

   gl_Position = vec4(P + N * normalLength,1.0);
   fcolours = vec3(1.0,0.0,0.0);
   EmitVertex();*/

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
