#version 150

in vec4 color; 
in vec2 uv;
out vec4 fragColor ; 

uniform sampler2D colormap;
uniform vec3 diffuse;

void main()
{

    /*!todo exercise 1: implement texture sampling {{{*/
    fragColor=vec4(1,1,1,1);
    /*}}}*/
}
