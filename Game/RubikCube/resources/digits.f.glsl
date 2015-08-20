#version 150

in vec2 uv;
out vec4 fragColor; 

uniform sampler2D colormap;

uniform vec2 uvShift;
void main()
{
  vec2 fuv=uv;
  fuv.y=fuv.y/2;
  fuv.x=fuv.x/5;
  fragColor=texture(colormap, fuv+uvShift);
}
