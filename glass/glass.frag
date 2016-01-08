#version 410

in vec2 ftextureCoords;
in vec3 fcolours;
in vec3 fnormal;

in vec3 ftoLightVector;
in vec3 ftoCameraVector;

out vec4 out_Color;
uniform sampler2D reflectionTexture;

const float shineDamper = 4.0;
const float reflectivity = 1.0;
const vec3 lightColour = {1.0,1.0,1.0};

void main () {

     vec3 unitNormal = normalize(fnormal);
     vec3 unitLightVector = normalize(ftoLightVector);

     float nDot1 = dot(unitNormal, unitLightVector);
     float brightness = max(nDot1, 0.0);
     vec3 diffuse = brightness * lightColour;

     vec3 unitVectorToCamera = normalize(ftoCameraVector);
     vec3 lightDirection = -unitLightVector;
     vec3 reflectedLightDirection = reflect(lightDirection,unitNormal);

     float specularFactor = dot(reflectedLightDirection, unitVectorToCamera);
     specularFactor = max(specularFactor,0.0);
     float dampedFactor = pow(specularFactor, shineDamper);
     vec3 finalSpecular = dampedFactor * lightColour;

     out_Color =  vec4(fcolours, 1.0) + + vec4(finalSpecular,1.0);
//     out_Color =  texture(reflectionTexture, ftextureCoords) + vec4(finalSpecular,1.0);

}
