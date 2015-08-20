uniform mat4 matrices[6];
uniform mat4 modelViewMatrix;
 
layout(triangles, invocations = 1) in;
layout(triangle_strip, max_vertices = 18) out;
 
in vec3 gPosition[];
in vec3 gNormal;
in vec3 gTangent;
in vec3 gBitangent;
in vec2 gUv;
in vec3 gColor; 
out vec3 position; //not an array since one position per emited vertex
out vec3 normal;
out vec3 tangent;
out vec3 bitangent;
out vec2 uv;
out vec3 color; 
out vec3 uvw;
 
void main() {
     
    for(int j = 0; j < 6; ++j) {
        gl_Layer = j;
        for(int i = 0; i < 3; ++i) {
            position = gPosition[i];
            normal = gNormal[i];
            tangent = gTangent[i];
            bitangent = gBitangent[i];
            uv = gUv[i];
            color = gColor[i];
            uvw = inverse(modelViewMatrix)*gPosition[i];
            gl_Position = matrices[j] * vec4(gPosition[i], 1);
            EmitVertex();
        }
        EndPrimitive();
    }
}
