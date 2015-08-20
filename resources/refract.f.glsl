#version 150

in vec4 position;
in vec3 normal;
in vec3 color;
noperspective in vec3 reflectedDir;
noperspective in vec3 refractedDir;
out vec4 fragColor ; 

uniform samplerCube cubemap;
uniform float reflectivity;
uniform float refractivity;

void main()
{
    vec3 reflection = texture(cubemap, reflectedDir).rgb;
    vec3 refraction = texture(cubemap, refractedDir).rgb;
    vec3 rcolor=mix(color, reflection, reflectivity);
    fragColor=vec4(mix(rcolor, refraction, refractivity), 1);
}

