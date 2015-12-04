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
#include <glm/gtc/constants.hpp>
//loading stuffs
#include "utils/utils.h"
#include "utils/shaders.h"
#include "utils/textures.h"
#include "utils/objloader.h"
#include "../include/lightGlAPI.hpp"
#include "mirror.hpp"/*}}}*/

unsigned int nbEx=7;
ensi::gl::Controls controls(nbEx);

//-------------------- MAIN /*{{{*/
int main(void)/*{{{*/
{
    init();
    mainLoop();
    shutDown(0);
}/*}}}*/

void mainLoop(void)/*{{{*/
{
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
    GLFWwindow* window=glfwCreateWindow(window_width, window_height, "Planar Mirror with Stencil Buffer and Clipping", NULL, NULL);
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
    //!Important: Set the attribute locations:
    // Now create a program (vertex:mirror, frag:mirror handles clipping)
    ensi::gl::Program prog("mirror.v.glsl", "mirror.f.glsl");
    ensi::gl::GLSLMaterial mat;
    mat.uniforms["diffuse"]=glm::vec3(1,1,1);
    mat.uniforms["specular"]=glm::vec3(0.7,0.7,0.7);
    mat.uniforms["shininess"]=500.;
    mat.uniforms["clipped"]=(int) 0;
    mat.program=prog;
    scene.materials["reflection"]=mat;
    // Create the scene/*{{{*/
    glm::mat4 mw;
    ensi::gl::Mesh cube;
    cube.makeMeCube(glm::vec4(1,0,0,1.));
    // floor/*{{{*/
    mw=glm::scale(mw, glm::vec3(8,8,0.1));
    scene.addObject("floor", cube, mw);
    /*}}}*/
    // wall/*{{{*/
    mw=glm::mat4();
    mw=glm::translate(mw, glm::vec3(0,mirrorYpos+0.05,0));
    mw=glm::scale(mw, glm::vec3(6,0.1,3));
    mw=glm::translate(mw, glm::vec3(0,0,0.5)); 
    scene.addObject("wall", cube, mw);
    //scene.addInstance("cube",  mw, "wall");
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
    /*}}}*/
    return 1;
}/*}}}*/
/*}}}*/


//-------------------- Rendering/*{{{*/
void drawAllButWallAndMirror(const std::string& materialName)/*{{{*/
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    scene.drawObject("suzanne", materialName);
    scene.drawObject("teapot", materialName);
    scene.drawObject("floor", materialName);
}/*}}}*/

void drawMirror(const std::string& materialName)/*{{{*/
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    scene.drawObject("mirror", materialName);
}/*}}}*/

void drawWall(const std::string& materialName)/*{{{*/
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    scene.drawObject("wall", materialName);
}/*}}}*/

void drawReflectionOfAllObjects(const std::string& materialName)/*{{{*/
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    //!todo: Clipping unwanted objects/*{{{*/
    if(controls.exercise>=3)
    {/*!todo 
      * 1- set the clipped uniform to 1
      * 2- Compute the plane equation in EYE coordinates (and send it to the GPU)
      * 3- Go in the fragment shader and deal with clipping there (by discarding non-valid fragments)
      * NB: Check the GLSLMaterial class
      */
    }/*}}}*/
    //!todo Reflecting all objects (except mirror)/*{{{*/
    if(controls.exercise>=2)
    {
        /*!todo there should be the following steps
         * 1- set the modelview matrix to account for the reflection
         * 2- draw the objects with :
         *    drawWall("reflection");
         *    drawAllButWallAndMirror("reflection");
         * 3- Think about face culling as well
         * NB: You can act on the matrix stack: controls.modelWoldMatrix to
         *    mirror the scene (check the function Programm::updateProgramUniforms
         *    to understand the role of this stack)
         * NB2: You can also obtain the local frame of the mirror thanks to
         *    scene.mwMatrices["mirror"]
         */
    }/*}}}*/
    //!todo Disabling the clip plane /*{{{*/
    if(controls.exercise>=3)
    {
        /*!todo */
    }/*}}}*/
}/*}}}*/

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
    //!note Nothing to do here /*{{{*/
    if(controls.exercise==0)
    {
        /*!note this is the simple situation where all object are
         * displayed normally (no stencil test)*/
        drawMirror("reflection");
        drawWall("reflection");
        drawAllButWallAndMirror("reflection");
    }/*}}}*/
    //!todo: Making a hole in the wall /*{{{*/
    else if(controls.exercise==1)
    {
        /*!todo in this situation, you have to:
        * 1- the mirror without writing in the color buffer nor in the z-buffer but
        *   only in the stencil buffer (set the concerned pixels to 1)
        * 2- draw the wall pixels, unless the stencil value is 1
        * 3- draw the rest of the scene normally
        * */
    }/*}}}*/
    //!todo: Reflecting the scene through the mirror /*{{{*/
    else if(controls.exercise==2 || controls.exercise==3)
    {
        /*!todo in this situation, simply restart from the exercise==0 section
         * (not exercise==1) and modify it to achieve the following:
         * 1- draw the mirror
         * 2- then call drawReflectionOfAllObjects function  (that you should
         *   modify) to draw the virtual objects (images of the real ones through
         *   the mirror)
         * 3- eventually, draw the  wall and the rest of the scene normally*/
    }/*}}}*/
    //!todo: Drawing the mirror in the Z-buffer ONLY /*{{{*/
    else if(controls.exercise==4)
    {
        /*!todo Here you simply need to render the mirror in the z-buffer only (do not overwrite colors)
         * Note, you must render virtual objects beforehand, so start from the previous section and modify it as follows:
         * 1- move the drawing of the mirror after the virtual objects
         * 2- modify  the color mask for the mirror rendering
         */
    }/*}}}*/
    //!todo: Discarding virtual objects outside the mirror field of view/*{{{*/
    else if(controls.exercise==5)
    {
        /*!todo 
         * 1- Write the mirror in the stencif buffer so as to set up the stencil
         *   buffer so as to discriminate between pixels of virtual objects in the
         *   mirror field of view and those outside
         * 2- Then you need to draw the virtual objects, taking care to discard
         *   all pixels falling ouside the mirror field
         * The rest remains identical to exercise 4.
         */
    }/*}}}*/
    //!todo Correcting the z_buffer in the mirror field of view/*{{{*/
    else if(controls.exercise==6)
    {
        /*!todo in this part, you should correct the way the z-buffer is
         * computed inside the mirror field of view; Two solutions are possible
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
    controls.mouseButtonCallback(button, action);
}/*}}}*/
/*}}}*/
// vim: foldmethod=marker autoread:
