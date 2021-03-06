#version 150
// ins (inputs)
in vec3 vertexPosition;
in vec4 vertexColor;
in vec2 vertexUV;
// out (outputs)
out vec4 color; 
out vec2 uv;
// uniforms
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    vec4 positionH=vec4(vertexPosition,1);
    gl_Position =  projectionMatrix * modelViewMatrix *  positionH;
    color=vertexColor;
    uv=vertexUV;
}
