#version 410

uniform float colour;
uniform float alpha;
out vec4 out_Color;

void main () {
        out_Color = vec4(colour,colour,colour,alpha);
}
