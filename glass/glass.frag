#version 410

in vec2 textureCoords;
out vec4 out_Color;
uniform sampler2D reflectionTexture;
uniform vec2 dot[4];

const float lineThickness = 0.01; //greate means bigger thickness

void main () {

    bool isOnTheLine;

    //draw dots at the designated coordinates
 /*   for(int i = 0; i < 4; i++){
        if(textureCoords.x < dot[i].x + 0.01 && textureCoords.x > dot[i].x + -0.01 &&
           textureCoords.y < dot[i].y + 0.01 && textureCoords.y > dot[i].y + -0.01){
            out_Color.xyz = vec3(0.0,0.0,0.0);
            isCoordinateMatched = true;
            break;
        }
    }*/

    //draw lines between the coordinates
    for(int i = 1; i < 4; i++){

        //for all of the dots,
        //find the equation of the line between two dots,
        bool isWithinBounds =false;
        float m = (dot[i].y - dot[i-1].y) / (dot[i].x - dot[i-1].x);
        float b = dot[i].y - m * dot[i].x;
        float y = m*textureCoords.x + b;


        //only draw the line in the area between the dots
        if(m >= 0){

            if(textureCoords.x  > dot[i-1].x && textureCoords.x < dot[i].x){
                isWithinBounds = true;
            }

        }else if ( m < 0){
            if(textureCoords.x  > dot[i].x && textureCoords.x < dot[i-1].x){
                isWithinBounds = true;
            }
        }

        if(isWithinBounds){
            //Check if the y coordindate false within the boundary of our line
            if(textureCoords.y < (y + lineThickness) && textureCoords.y > (y - lineThickness)){
                out_Color.xyz = vec3(1.0,0.0,0.0);
                isOnTheLine = true;
                break;
            }
        }
    }

    if(!isOnTheLine){
        out_Color = texture(reflectionTexture, textureCoords);
    }
}
