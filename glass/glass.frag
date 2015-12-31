#version 410

in vec2 textureCoords;
out vec4 out_Color;

uniform sampler2D reflectionTexture;

void main () {
    out_Color = texture(reflectionTexture, textureCoords);
}
