#version 150

//uniform sampler2D colormap;
//uniform sampler2D normalmap; 
uniform float time;
// clipping
uniform int clipped;
uniform vec4 clipplaneEqn;
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
in vec3 tangent; 
in vec3 bitangent;
in vec3 color; 
in vec2 uv;
out vec4 fragColor; 



vec3 ComputeLightLambert(const in vec3 lightdirn, const in vec3 lightcolor, const in vec3 normal, const in vec3 mydiffuse)/*{{{*/
{
    float nDotL = dot(normal, lightdirn);
    vec3 color=lightcolor;
    vec3 lambert = mydiffuse * color * max (nDotL, 0.0);  
    return lambert;
}/*}}}*/


vec3 ComputeLightSpecular (const in vec3 lightdirn, const in vec3 lightcolor, const in vec3 normal, const in vec3 eyedirn, const in vec3 myspecular, const in float myshininess) /*{{{*/
{
    vec3 halfvec = normalize (lightdirn + eyedirn); 
    float nDotH = dot(normal, halfvec); 
    vec3 color=lightcolor;
    vec3 phong = myspecular * color * pow (max(nDotH, 0.0), myshininess); 
    return phong;
}/*}}}*/

void main()
{
    if(bool(clipped))
    {
        /*!todo: compute the signed distance between the plane and the vertex
         *   the plane equation is stored in clipplaneEqn and is expressed in the eye coordinate system
         *   the vertex is stored in the input "position" and is expressed in the eye coordinate system
         */ 
    }
  vec3 fragNormal = normal;
  fragNormal = normalize(fragNormal); 
  vec3 lambert = ComputeLightLambert(lightdirn, lightcolor, fragNormal, diffuse);
  const vec3 eyepos = vec3(0,0,0); 
  vec3 eyedirn = normalize(eyepos - position.xyz/position.w);// don't forget to dehomogenize the vertex
  float myshininess=shininess*(.8+ sin(time) * 0.5f);
  vec3 phong= ComputeLightSpecular(lightdirn, lightcolor, fragNormal, eyedirn, specular, myshininess);
  fragColor=vec4(ambient*color,1);
  fragColor= fragColor+vec4(lambert,1);
  fragColor= fragColor+vec4(phong,1);
}
