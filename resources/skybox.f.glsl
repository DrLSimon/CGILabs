#version 150
//#extension GL_ARB_texture_cube_map : require

in vec3 uvw;
out vec4 fragColor ; 

uniform samplerCube cubemap;

void main()
{
    fragColor=texture(cubemap, uvw);
}
