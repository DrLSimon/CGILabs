#version 150
// ins (inputs)
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexColor;
in vec3 vertexTangent;
in vec2 vertexUV;
// out (outputs)
out vec4 position;
out vec3 normal;
out vec3 uvw;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
uniform float time;

void main()
{
    vec4 positionH=vec4(vertexPosition,1);
    gl_Position =  projectionMatrix * modelViewMatrix *  positionH;
    position=modelViewMatrix * positionH;
    normal=normalMatrix * vertexNormal;
    normal=normalize(normal);
    uvw=vertexPosition;
}
