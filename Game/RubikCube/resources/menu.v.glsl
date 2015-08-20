  #version 150
// ins (inputs)
in vec3 vertexPosition;
in vec2 vertexUV;
// out (outputs)
out vec2 uv;

uniform mat4 modelWorldMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position =  projectionMatrix*modelWorldMatrix*vec4(vertexPosition, 1);
    uv=vertexUV;
}
