/* Include headers*//*{{{*/
#include <stdlib.h>
#include <iostream>
#include <stack>
#include <vector>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
// matrix and vectors
// to declare before including glm.hpp, to use the swizzle operators
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//loading stuffs
#include "utils/utils.h"
#include "utils/shaders.h"
#include "utils/textures.h"
#include "utils/objloader.h"
#include "../include/lightGlAPI.hpp"
#include "shadows.hpp"/*}}}*/

unsigned int nbEx=5;
ensi::gl::Controls controls(nbEx);


//-------------------- ShadowVolume classes/*{{{*/
class Edge {/*{{{*/
public:
    Edge(const glm::vec3& p1, const glm::vec3& p2):pt1(p1),pt2(p2){}
    glm::vec3 pt1;
    glm::vec3 pt2;

    Edge  swap() const/*{{{*/
    {
        return Edge(pt2,pt1);
    }/*}}}*/

    bool operator< ( const Edge& other) const/*{{{*/
    {
        // a comparison operator to use set<glm::vec3>
        return std::lexicographical_compare(glm::value_ptr(pt1),glm::value_ptr(pt1)+3,
                                            glm::value_ptr(other.pt1),glm::value_ptr(other.pt1)+3) ||
            (pt1==other.pt1 &&
             std::lexicographical_compare(glm::value_ptr(pt2),glm::value_ptr(pt2)+3,
                                          glm::value_ptr(other.pt2),glm::value_ptr(other.pt2)+3));
    }/*}}}*/
};/*}}}*/

typedef std::set<Edge> EdgeSet; // this structure will be useful to create sets of edges

class ShadowVolumeBuilder/*{{{*/
{
public:
    ShadowVolumeBuilder(float mag=10.):/*{{{*/
        extrudeMagnitude(mag)
    {
    }/*}}}*/

    virtual ~ShadowVolumeBuilder()/*{{{*/
    {
    }/*}}}*/

    void capShadowVolume(const ensi::gl::Mesh& shadowCaster, const glm::vec3& lightdir, ensi::gl::Mesh& shadowVolume)/*{{{*/
    {
        ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
        //!todo: Cap the shadow volume /*{{{*/
        if(controls.exercise>=1)
        {
            /*! Here you have to complete the shadow volume with the faces of the shadow caster that are directed toward the light
             */
        }/*}}}*/
    }/*}}}*/

    void capShadowVolumeBack(const ensi::gl::Mesh& shadowCaster, const glm::vec3& lightdir, ensi::gl::Mesh& shadowVolume)/*{{{*/
    {
        ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
        //!todo: Cap the shadow volume at the "far" end/*{{{*/
        if(controls.exercise>=2 and controls.exercise!=3)
        {
            /*! No indication here, you have to find out by yourself
             */
        }/*}}}*/
    }/*}}}*/
    ensi::gl::Mesh buildShadowVolume(const ensi::gl::Mesh& shadowCaster, const glm::vec3& lightdir)/*{{{*/
    {
        ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
        ensi::gl::Mesh shadowVolume;
        EdgeSet edges=getSilhouetteEdges(shadowCaster, lightdir);
        //!todo: Extruded volume /*{{{*/
        if(controls.exercise>=1)
        {
            /*! Here you should extrude all the contour edges in the light direction in order to create the side facets of the shadow volume
             * Each edge will be transformed into a quad (actually two triangles)
            */
        }/*}}}*/
        return shadowVolume;
    }/*}}}*/

protected:
    EdgeSet getSilhouetteEdges(const ensi::gl::Mesh& shadowCaster, const glm::vec3& lightdir)/*{{{*/
    {
        ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
        EdgeSet edges;
        //!todo: Silhouette edges  /*{{{*/
        if(controls.exercise>=1)
        {
            /*! In this part, you have to extract the edges where the light beams are tangent to the object
             * These edges constitute the contour of the object when seen from the light source
             * Therefore they have a key role in determining the shadow volume casted by the object
             * These edges can be determined easily since they are shared by two faces: one of them is directed toward the source and the other one in the opposite direction
             *
             * NB the EdgeSet data structure will be convenient to store the edges; You can:
             *   add an element with the method
             *      void EdgeSet::insert(const Edge& edge)
             *   find an element with the method
             *      EdgeSet::iterator EdgeSet::find(const Edge& edge)
             *   and remove an element with the method
             *      void edges.erase(EdgeSet::iterator pos)
             */
        }/*}}}*/
        return edges;
    }/*}}}*/

private:
    float extrudeMagnitude;
};/*}}}*/
ShadowVolumeBuilder svb(15.);
/*}}}*/



//-------------------- MAIN /*{{{*/
int main(void)/*{{{*/
{
    init();
    mainLoop();
    shutDown(0);
}/*}}}*/

void mainLoop(void)/*{{{*/
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    GLFWwindow* window=controls.getWindow();
    // this just loops as long as the program runs
    while(!glfwWindowShouldClose(window))
    {
        /* Calculate time elapsed, and the amount by which stuff rotates*//*{{{*/
        double current_time = glfwGetTime();
        controls.setTime(current_time);
        controls.updateView();
        /*}}}*/

        /* Additional key handling (when repeat key is needed)*//*{{{*/
        // escape to quit, arrow keys to rotate view
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS or glfwGetKey(window, 'Q') == GLFW_PRESS)
            break;
        controls.continuousKeyCallback();
        /*}}}*/

        /* Send all the drawing commands and swap the buffers*//*{{{*/
        draw();
        // swap back and front buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        controls.resetView();
        /*}}}*/
    }
}/*}}}*/
/*}}}*/


//-------------------- Inits/*{{{*/
void init(void)/*{{{*/
{ 
    //-------------------- Globals/*{{{*/
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    ensi::gl::Renderer* renderer=new ensi::gl::GLRenderer();
    scene.setRenderer(renderer);
    scene.setControls(&controls);
    /*}}}*/
    /* GLFW initialization *//*{{{*/
    const int window_width = 1024, window_height = 800;
    if (!glfwInit())
        shutDown(1);
    // Create OpenGL 3.x Core Profile Context                                                   
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
#endif
    // allocate 8 bits for the stencil buffer
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // 800 x 600, windowed
    GLFWwindow* window=glfwCreateWindow(window_width, window_height, "Shadow volumes (via stencil buffer)", NULL, NULL);
    controls.setWindow(window);
    if (!window)
    {
        std::cerr<<"Could not open a window"<<std::endl;
        shutDown(1);
    }
    glfwMakeContextCurrent(window);
    /*}}}*/
    
    /* GLEW Initialization *//*{{{*/
    // Do not forget to use glewExperimental so that glewInit works with core
    // profile: http://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();
    std::cout<<"Here, we should expect to get a GL_INVALID_ENUM (that's a known bug), and indeed:"<<std::endl;
    checkGLerror();
    if (GlewInitResult != GLEW_OK) {
        std::cerr<<"ERROR: "<< glewGetErrorString(GlewInitResult)<<std::endl;
        shutDown(1);
    }/*}}}*/
    
    /* GLFW callbak definitions *//*{{{*/
    glfwSetFramebufferSizeCallback(window, &resize);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    /*}}}*/
    
    /* Creating the controls*//*{{{*/
    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);
    resize(window, width, height);
    /*}}}*/

    /* Creating OpenGL resources *//*{{{*/
    make_resources();
    std::cout<<"Seems we made it "<<std::endl;
    std::cout<<"OpenGL version: "<<glGetString(GL_VERSION)<<std::endl;
    std::cout<<"GLSL version: "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<std::endl;/*}}}*/
}/*}}}*/

int make_resources(void)/*{{{*/
{ 
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    float mirrorYpos=1.5;
    // Now create a program (vertex:simple, frag:normal)
    ensi::gl::Program prog("illumination.v.glsl", "illumination.f.glsl");
    ensi::gl::GLSLMaterial material;
    material.uniforms["diffuse"]=glm::vec3(1,1,1);
    material.uniforms["specular"]=glm::vec3(0.7,0.7,0.7);
    material.uniforms["shininess"]=500.;
    material.uniforms["exercise"]=(unsigned int)3;
    material.program=prog;
    scene.materials["phong"]=material;
    // ambient shader
    material.uniforms["exercise"]=(unsigned int)0;
    scene.materials["ambient"]=material;
    // Create the scene/*{{{*/
    glm::mat4 mw;
    ensi::gl::Mesh cube;
    cube.makeMeCube(glm::vec4(1,0,0,1.));
    // floor/*{{{*/
    mw=controls.modelWorldMatrix.top();
    mw=glm::scale(mw, glm::vec3(8,8,0.1));
    scene.addObject("floor", cube, mw);
    /*}}}*/
    // wall/*{{{*/
    mw=glm::mat4();
    mw=glm::translate(mw, glm::vec3(0,mirrorYpos+0.05,0));
    mw=glm::scale(mw, glm::vec3(6,0.1,3));
    mw=glm::translate(mw, glm::vec3(0,0,0.5)); 
    scene.addObject("wall", cube, mw);
    /*}}}*/
    // mirror/*{{{*/
    mw=glm::mat4();
    mw=glm::translate(mw, glm::vec3(0,mirrorYpos,0.9));
    mw=glm::scale(mw, glm::vec3(3,0.01,2));
    mw=glm::translate(mw, glm::vec3(0,0,0.5));
    cube.makeMeCube(glm::vec4(1,1,0,1.));
    scene.addObject("mirror", cube, mw);/*}}}*/
    // suzanne/*{{{*/
    mw=glm::mat4();
    mw=glm::translate(mw, glm::vec3(1.25,0.25,1.));
    mw=glm::rotate(mw, 90.f, glm::vec3(1,0,0));
    mw=glm::rotate(mw, -160.f, glm::vec3(0,1,0));
    ensi::gl::Mesh suzanne;
    suzanne.loadObj("suzanne.obj", glm::vec4(0.1,0.1,1,1));
    scene.addObject("suzanne", suzanne, mw);/*}}}*/
    // teapot/*{{{*/
    mw=glm::mat4();
    mw=glm::translate(mw,glm::vec3(0,mirrorYpos+1,0.7));
    mw=glm::rotate(mw, 45.f, glm::vec3(0,1,1));
    ensi::gl::Mesh teapot;
    teapot.loadObj("teapot.obj", glm::vec4(1,0.1,1,0.7));
    scene.addObject("teapot", teapot, mw);/*}}}*/
    // torus/*{{{*/
    mw=glm::mat4();
    mw=glm::translate(mw, glm::vec3(-1.25,0.25,1.));
    mw=glm::rotate(mw, 70.f, glm::vec3(1,0,0));
    mw=glm::rotate(mw, -160.f, glm::vec3(0,1,0));
    mw=glm::scale(mw, glm::vec3(1.3,1.2,1));
    ensi::gl::Mesh torus;
    torus.loadObj("torus.obj", glm::vec4(0.1,1,0.1,1));
    scene.addObject("torus", torus, mw);/*}}}*/
    scene.renderer->setPicked("torus");
    /*}}}*/
    return 1;
}/*}}}*/
/*}}}*/


//-------------------- Rendering/*{{{*/
void drawShadowVolume(const std::string& name, const std::string& progname)/*{{{*/
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    std::string nameSV=name+"SV";
    const ensi::gl::Mesh& mesh=scene.objects[name]->asMesh();
    glm::mat4 mw=scene.mwMatrices[name];
    glm::vec3 lightdirWorld=controls.getLightDir();
    glm::vec3 lightdirModel=glm::inverse(glm::mat3(mw))*lightdirWorld;
    ensi::gl::Mesh svMesh=svb.buildShadowVolume(mesh,lightdirModel);
    svb.capShadowVolume(mesh, lightdirModel, svMesh);
    svb.capShadowVolumeBack(mesh, lightdirModel, svMesh);
    scene.addObject(nameSV, svMesh, mw);
    glDepthMask(GL_FALSE); // a transparent object should not write in the z-buffer
    scene.drawObject(nameSV, progname);
    glDepthMask(GL_TRUE);
}/*}}}*/

void drawScene(const std::string& progname)/*{{{*/
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    std::vector<std::string> objectnames={"suzanne","teapot","floor","torus","wall","mirror"};
    for (unsigned int  i = 0; i < objectnames.size(); ++i)
    {
        std::string& name =objectnames[i];
        if(name!=scene.renderer->getPicked() or controls.exercise!=2)
            scene.drawObject(name, progname, false);
    }
}
/*}}}*/

void draw()/*{{{*/
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT );
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearStencil( 0x0 );
    glClear(GL_STENCIL_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    if(controls.exercise==0)
    {
        drawScene("phong");
    }
    else if(controls.exercise==1 or controls.exercise==2)
    {
        drawScene("phong");
        glDisable(GL_CULL_FACE);
        drawShadowVolume(scene.renderer->getPicked(), "ambient");
        glEnable(GL_CULL_FACE);
    }
    //!todo: z-Pass shadow volume /*{{{*/
    else if(controls.exercise==3)
    {
        /*! Here you will perform rendering to implement the z-pass shadow volume technique, that goes as follows
         * 1) Draw the normal scene in ambiant mode (for this you have to set the ambientonly uniform to 1 (set it back to 0 afterward)
         * 2) Draw the shadow volume in the stencil buffer only, and at the condition that the z-test passes; You will use the following rule
         *   - front faces increment the stencil
         *   - back faces decrement the stencil
         * NB: For this step you'll need to enable alternatively back face culling and front face culling (google is your friend)
         * 3) Draw the normal scene in normal lighting mode, taking care that since it was already rendered in z-buffer you should change the z-test function to GL_EQUAL (this is an alternative to clearing the z-buffer); Do not forget to set it back to its original settings (GL_LESS).
         */
    }/*}}}*/
    //!todo: z-Fail shadow volume /*{{{*/
    else if(controls.exercise>=4)
    {
        /*! Here you will perform rendering to implement the z-fail shadow volume technique, that goes as follows
         * 1) Draw the normal scene in ambient mode  
         * 2) Draw the shadow volume in the stencil buffer only, and at the
         * condition that the z-test fails; You will use the following rule
         *   - back faces increment the stencil
         *   - front faces decrement the stencil
         * NB: For this step you'll need to enable alternatively back face culling and front face culling (google is your friend)
         * 3) Draw the normal scene in normal lighting mode, taking care that
         * since it was already rendered in z-buffer you should change the
         * z-test function to GL_EQUAL (this is an alternative to clearing the
         * z-buffer); Do not forget to set it back to its original settings (GL_LESS).
         */
    }/*}}}*/
    checkGLerror();
    glDisable(GL_DEPTH_TEST);
}/*}}}*/

/*}}}*/


//-------------------- GLFW Callbacks/*{{{*/
void shutDown(int return_code)/*{{{*/
{
    glfwTerminate();
    exit(return_code);
}/*}}}*/

void resize(GLFWwindow* window, int width, int height)/*{{{*/
{        
    controls.resize(width, height);
}/*}}}*/

void keyCallback(GLFWwindow* window, int key, int scancode,  int action, int modifiers)/*{{{*/
{
    controls.keyCallback(key, action, modifiers);
}/*}}}*/

void mouseButtonCallback(GLFWwindow* window, int button, int action, int modifiers)/*{{{*/
{
    const ensi::gl::Camera& camera=controls.getCamera();
    controls.picker.pickableNames={"torus", "floor", "wall", "mirror", "suzanne", "teapot"};
    if(button==GLFW_MOUSE_BUTTON_LEFT)
    {
        if(action==GLFW_PRESS){
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            x*=camera.screenToPixel[0];
            y*=camera.screenToPixel[1];
            ensi::gl::Scene& scene=ensi::gl::Scene::getInstance();
            std::string pickedname=controls.picker.performPicking(scene, x,  y);
            scene.renderer->setPicked(pickedname);
            std::cout << pickedname  << " was picked!" << std::endl;
        }
    }
}/*}}}*/
/*}}}*/
// vim: foldmethod=marker autoread:
