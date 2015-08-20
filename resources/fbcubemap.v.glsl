#version 150
// ins (inputs)
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexColor;
in vec2 vertexUV;
in vec3 vertexTangent;
// out (outputs)
out vec4 gPosition;
out vec3 gNormal;
out vec3 gTangent;
out vec3 gBitangent;
out vec2 gUv;
out vec3 gColor; 
out vec3 gUvw;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;


// all those uniforms are only there for gouraud shading (otherwise they are only in th fragment shader) 
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main()
{

    vec4 positionH=vec4(vertexPosition,1);
    gl_Position =  projectionMatrix * modelViewMatrix *  positionH;
    gPosition=modelViewMatrix * positionH;
    gNormal=normalMatrix * vertexNormal;
    gNormal=normalize(gNormal);
    gTangent=mat3(modelViewMatrix) * vertexTangent;
    gTangent=normalize(gTangent);
    gBitangent=cross(gNormal, gTangent);
    gColor=vertexColor;
    gUv=vertexUV; 
    gUvw=vertexPosition;
}

