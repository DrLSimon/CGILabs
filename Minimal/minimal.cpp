/* Include headers*//*{{{*/
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
//loading stuffs
#include "utils/utils.h"
#include "utils/shaders.h"
#include "utils/textures.h"
#include "utils/objloader.h"
#include "../include/lightGlAPI.hpp"
#include "minimal.hpp"/*}}}*/

unsigned int nbEx=4;
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
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // 800 x 600, default bit color, 32 bit depth, no alpha or stencil buffers, windowed
    GLFWwindow* window=glfwCreateWindow(window_width, window_height, "Minimal OpenGL application with GLFW", NULL, NULL);
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
    //!Important: Set the attribute locations:
    // Now create a program
    ensi::gl::Program prog("simple.v.glsl", "simple.f.glsl");
    // Create a material
    ensi::gl::GLSLMaterial material;
    material.program=prog;
    scene.materials["simple"]=material;
    // Create the scene/*{{{*/
    glm::mat4 mw;
    ensi::gl::Mesh cube;
    makeAPerFaceColoredCube(cube);
    // cube/*{{{*/
    mw=controls.modelWorldMatrix.top();
    mw=glm::scale(mw, glm::vec3(2,2,2));
    scene.addObject("cube", cube, mw);/*}}}*/
    // torus/*{{{*/
    ensi::gl::Mesh torus;
    makeAColoredTorus(torus, 1, 0.25, 50);
    mw=controls.modelWorldMatrix.top();
    mw=glm::scale(mw, glm::vec3(2,2,2));
    mw=glm::rotate(mw, glm::pi<float>()/4, glm::vec3(2,2,2));
    scene.addObject("torus", torus, mw);/*}}}*/
    /*}}}*/
    return 1;
}/*}}}*/
/*}}}*/


void draw()/*{{{*/
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT );
    if(controls.exercise>=1)
    {
        /*!todo exercise 1: Enable back face culling *//*{{{*/
        /*}}}*/
    }
    if(controls.exercise>=3)
    {
        /*!todo exercise 3: Enable depth test *//*{{{*/
        /*}}}*/
    }
    scene.drawObject("cube","simple");
    if(controls.exercise>=2)
        scene.drawObject("torus","simple");
    checkGLerror();
    if(controls.exercise>=1)
    {
        /*!todo exercise 1: Disable back face culling *//*{{{*/
        /*}}}*/
    }
    if(controls.exercise>=3)
    {
        /*!todo exercise 3: Disable depth test *//*{{{*/
        /*}}}*/
    }
}
/*}}}*/

/*Geometry construction {{{*/
void makeAPerFaceColoredCube(ensi::gl::Mesh& cube)/*{{{*/
{
    using ensi::gl::Vertex;
    cube.m_verts.clear();
    cube.m_tris.clear();
    std::vector<glm::vec3> positions
    {
        glm::vec3(0,0,0), glm::vec3(1,0,0), glm::vec3(1,1,0), glm::vec3(0,1,0),
        glm::vec3(0,0,1), glm::vec3(1,0,1), glm::vec3(1,1,1), glm::vec3(0,1,1),
        glm::vec3(0,0,0), glm::vec3(0,1,0), glm::vec3(0,1,1), glm::vec3(0,0,1),
        glm::vec3(1,0,0), glm::vec3(1,1,0), glm::vec3(1,1,1), glm::vec3(1,0,1),
        glm::vec3(0,0,0), glm::vec3(1,0,0), glm::vec3(1,0,1), glm::vec3(0,0,1),
        glm::vec3(0,1,0), glm::vec3(1,1,0), glm::vec3(1,1,1), glm::vec3(0,1,1)
    };
    std::vector<glm::vec4> colors=
    {
        glm::vec4(1,0,0,1),glm::vec4(1,0,0,1),glm::vec4(1,0,0,1),glm::vec4(1,0,0,1),
        glm::vec4(1,1,0,1),glm::vec4(1,1,0,1),glm::vec4(1,1,0,1),glm::vec4(1,1,0,1),
        glm::vec4(0,1,0,1),glm::vec4(0,1,0,1),glm::vec4(0,1,0,1),glm::vec4(0,1,0,1),
        glm::vec4(0,1,1,1),glm::vec4(0,1,1,1),glm::vec4(0,1,1,1),glm::vec4(0,1,1,1),
        glm::vec4(0,0,1,1),glm::vec4(0,0,1,1),glm::vec4(0,0,1,1),glm::vec4(0,0,1,1),
        glm::vec4(1,0,1,1),glm::vec4(1,0,1,1),glm::vec4(1,0,1,1),glm::vec4(1,0,1,1),
    };
    std::vector<GLuint> ibo=
    {
        0,2,1, 0,3,2,
        4,5,6, 4,6,7,
        8,10,9, 8,11,10,
        12,13,14, 12,14,15,
        16,17,18, 16,18,19,
        20,22,21, 20,23,22
    };
    for (int i = 0; i < 24; ++i)
    {
        cube.m_verts.push_back(Vertex(positions[i], glm::vec3(), glm::vec2(), colors[i]));
    }
    cube.m_tris=ibo;
}
/*}}}*/


inline glm::vec3 torusPoint(double theta, double phi, double R, double r)/*{{{*/
{
    double D=(R + r*cos(phi));
    return glm::vec3(D*cos(theta), D*sin(theta), r*sin(phi));
}/*}}}*/

inline glm::vec3 torusNormal(double theta, double phi)/*{{{*/
{
    return glm::vec3 (cos(phi)*cos(theta), cos(phi)*sin(theta), sin(phi));
}/*}}}*/

void makeAColoredTorus(ensi::gl::Mesh& torus, double R, double r, int nbBins)/*{{{*/
{
    using ensi::gl::Vertex;
    torus.m_verts.clear();
    torus.m_tris.clear();
    /*!todo exercise 2: build up a torus *//*{{{*/
    /*}}}*/
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
