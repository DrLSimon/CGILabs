#version 150
// ins (inputs)
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexColor;
// out (outputs)
out vec4 position;
out vec3 normal;
out vec3 color;
//!note: perspective correction actually screw up here
noperspective out vec3 reflectedDir;
noperspective out vec3 refractedDir;


uniform mat4 modelViewMatrix;
uniform mat4 modelWorldMatrix;
uniform mat4 worldViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 cubemapMatrix;// that's the inverse modelWorld For the environment map
uniform mat3 normalMatrix;

void main()
{
    vec4 positionH=vec4(vertexPosition,1);
    gl_Position =  projectionMatrix * modelViewMatrix *  positionH;
    // COmpute everything in environment map model space
    position=cubemapMatrix*modelWorldMatrix * positionH;
    vec4 eyePos=cubemapMatrix*inverse(worldViewMatrix)*vec4(0,0,0,1);
    vec3 eyeDir=normalize(position.xyz/position.w-eyePos.xyz/eyePos.w);
    normal=inverse(transpose(mat3(cubemapMatrix*inverse(worldViewMatrix))))*normalMatrix * vertexNormal;
    normal=normalize(normal);
    reflectedDir=reflect(eyeDir, normal);
    refractedDir=refract(eyeDir, normal, 1/1.33);
    color=vertexColor;
}

