#include <stdlib.h>
#include <iostream>
#include <stack>
#include <vector>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
// matrix and vectors
// to declare before including glm.hpp, to use the swizzle operators
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include "../include/lightGlAPI.hpp"

using namespace ensi::gl;
std::map<std::string, GLint> Program::attributes;

void Program::setAttributeLocations(){/*{{{*/
    unsigned int i=0;
    for(const std::string& name : {"vertexPosition","vertexColor","vertexNormal","vertexTangent","vertexUV"})
    {
        attributes[name]=i++;
    }
}/*}}}*/



Scene& Scene::getInstance(){/*{{{*/
    if(! instance)
    {
        instance = new Scene();
    }
    return *instance;
}/*}}}*/

Scene* Scene::instance=NULL;

Renderer::Renderer ():scene(Scene::getInstance())/*{{{*/
{
}/*}}}*/

void Controls::mouseButtonCallback(int button, int action)/*{{{*/
{
    const Camera& camera=getCamera();
    GLFWwindow* window=camera.window;
    if(button==GLFW_MOUSE_BUTTON_LEFT)
    {
        if(action==GLFW_PRESS){
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            x*=camera.screenToPixel[0];
            y*=camera.screenToPixel[1];
            Scene& scene=Scene::getInstance();
            std::string pickedname=picker.performPicking(scene, x,  y);
            scene.renderer->setPicked(pickedname);
            std::cout << pickedname  << " was picked!" << std::endl;
        }
    }
}
/*}}}*/                                                                               

std::string Picker::performPicking(const Scene& scene, int x, int y) const/*{{{*/
{
    const IControls& controls=*scene.controls;
    std::string pickedname="";
    float distNearest=std::numeric_limits<float>::max();
    /*!todo: Picking Lab Exercise 2: Compute the ray in Eye coordinate space {{{*/
    /*! 1 - get the NDC coords of (x,y) at the near plane 
     *      Beware y is inversed use height-y-1 instead
     *  2 - Get the cooresponding clipping coords (beware that we deal
     *      with a vector not a point)
     *  3 - Get the eye coords of the ray direction (raydirEye)
     *  4 - Same for the ray origin (rayorigEye)
     *  5 - provide a consistent value for tMin
     *  NB: You may place the ray origin at the camera center or at
     *      the near plane
     * */
    //! This are the three variable to compute
    glm::vec4 rayorigEye(0,0,0,1);
    glm::vec3 raydirEye(0,0,-1);
    float tMin=0;
    /*}}}*/
    const glm::mat4& wv=controls.getViewMatrix()*controls.getWorldMatrix();
    for(auto& entry : scene.objectnames)
    {
        float distHit=std::numeric_limits<float>::max();
        const std::string& instancename=entry.first;
        if( (not pickableNames.empty()) and (pickableNames.find(instancename)==pickableNames.end()))
            continue;
        Ray ray(rayorigEye, raydirEye, tMin);
        if (not intersectInstance(scene, instancename, ray, wv,  distHit))
            continue;
        if(distHit<distNearest)
        {
            distNearest=distHit;
            pickedname=instancename;
        }
    }
    return pickedname;
}
/*}}}*/

bool Picker::intersectInstance(const Scene& scene, const std::string& instancename, const Ray& ray, const glm::mat4& wv, float& distHit) const/*{{{*/
{
    const std::string& objectname=scene.objectnames.at(instancename);
    const SceneObject& object=*scene.objects.at(objectname);
    // compute the ray in Model coords
    glm::mat4 vm=glm::inverse(wv*scene.mwMatrices.at(instancename));

    Ray rayInModel(ray);
    rayInModel.changeFrame(vm);
    bool reachable=object.intersectBoundingSphere(rayInModel, distHit);
    if(not reachable)
        return false;
    return object.intersect(rayInModel, distHit);
    return false;
}
/*}}}*/



/******************************************************************************
 *                              Debug operators                               *
 ******************************************************************************/

std::ostream& operator<<(std::ostream& out, const ensi::gl::Vertex& v){/*{{{*/
    out<<"position :"<<v.position.x<<", "<<v.position.y<<", "<<v.position.z<<std::endl;
    out<<"normal :"<<v.normal.x<<", "<<v.normal.y<<", "<<v.normal.z<<std::endl;
    out<<"tangent :"<<v.tangent.x<<", "<<v.tangent.y<<", "<<v.tangent.z<<std::endl;
    out<<"color :"<<v.color.r<<", "<<v.color.g<<", "<<v.color.b<<", "<<v.color.a<<std::endl;
    out<<"uv :"<<v.uv.x<<", "<<v.uv.y;
    return out;
}/*}}}*/

std::ostream& operator<<(std::ostream& out, const ensi::gl::Mesh& m){/*{{{*/
    unsigned int i = 0;
    for(const ensi::gl::Vertex& v : m.m_verts)
    {
        out<<"Vertex "<<i++<<std::endl<<v<<std::endl;
    }
    for(unsigned int i = 0; i < m.m_tris.size(); i+=3)
    {
        out<<"Triangle "<<i/3<<": "<<std::endl
            <<m.m_tris[i]<<" "<<m.m_tris[i+1]<<" "<<m.m_tris[i+2]<<std::endl;
    }
    return out;
}/*}}}*/

std::ostream& operator<<(std::ostream& out, const ensi::gl::Program& p){/*{{{*/
    std::cout<<"Attributes:"<<std::endl;
    for(auto& entry : p.attributes)
    {
        const std::string& name=entry.first;
        GLint& index=entry.second;
        out<<"\t"<<name<<" -> "<<index<<std::endl;
    }
    return out;
}/*}}}*/

void displayMat4(const glm::mat4& m)/*{{{*/
{
    std::cout << m[0][0] << " " << m[0][1] << " " << m[0][2] << " " << m[0][3] << std::endl 
              << m[1][0] << " " << m[1][1] << " " << m[1][2] << " " << m[1][3] << std::endl 
              << m[2][0] << " " << m[2][1] << " " << m[2][2] << " " << m[2][3] << std::endl 
              << m[3][0] << " " << m[3][1] << " " << m[3][2] << " " << m[3][3] << std::endl;
}/*}}}*/
