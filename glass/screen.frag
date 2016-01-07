#version 410

uniform float colour;
uniform float alpha;
out vec4 out_Color;

void main () {

        float col = clamp(colour, 0.0,1.0);
        float alp = clamp(alpha, 0.0, 1.0);
        out_Color = vec4(col,col,col,alp);
}
