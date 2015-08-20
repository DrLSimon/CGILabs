#version 150

uniform sampler2D colormap;
uniform samplerCube cubemap;
uniform float time;
// Assume light is directional
uniform vec3 lightdirn; 
uniform vec3 lightcolor; 
// material properties
uniform vec3 ambient; 
uniform vec3 diffuse; 
uniform vec3 specular; 
uniform float shininess; 
in vec4 position;
in vec3 normal; 
in vec2 uv;
in vec3 uvw;
out vec4 fragColor; 



void main()
{
    vec4 albedo=texture(cubemap, uvw);
    fragColor= albedo;
}
