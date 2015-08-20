#version 150
#extension GL_ARB_separate_shader_objects: require
// ins (inputs)
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexColor;
in vec2 vertexUV;
in vec3 vertexTangent;
// out (outputs)
out vec4 position;
out vec3 normal;
out vec3 tangent;
out vec3 bitangent;
out vec2 uv;
out vec3 color; 

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
uniform float time;
// Assume light is directional
uniform vec3 lightdirn; 
uniform vec3 lightcolor; 
// material properties
uniform vec3 ambient; 
uniform vec3 diffuse; 
uniform vec3 specular; 
uniform float shininess; 

void main()
{
    vec4 positionH=vec4(vertexPosition,1);
    gl_Position =  projectionMatrix * modelViewMatrix *  positionH;
    position=modelViewMatrix * positionH;
    normal=normalMatrix * vertexNormal;
    normal=normalize(normal);
    tangent=mat3(modelViewMatrix) * vertexTangent;
    tangent=normalize(tangent);
    bitangent=cross(normal, tangent);
    color=vertexColor;
    uv=vertexUV; 
}
