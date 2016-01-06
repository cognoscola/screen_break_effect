#version 410

in vec2 ftextureCoords;
in vec3 fcolours;

out vec4 out_Color;
uniform sampler2D reflectionTexture;

void main () {

          out_Color = vec4(fcolours, 1.0);
//        out_Color = texture(reflectionTexture, ftextureCoords);
}
