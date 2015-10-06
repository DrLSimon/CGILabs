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
#include "textures.hpp"/*}}}*/

unsigned int nbEx=5;
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
    GLFWwindow* window=glfwCreateWindow(window_width, window_height, "Texture mapping", NULL, NULL);
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
    // Create the scene/*{{{*/
    glm::mat4 mw;
    /* Dice material {{{*/
    ensi::gl::GLSLMaterial mat;
    // NB: tga files con be obtained with the imagemagick conversion tool as follows
    //     convert checkerboard.jpg -type TrueColor ../checkerboard.tga
    mat.uniformSamplers["dicetexture"]=loadTGATexture("dice.tga");
    mat.uniforms["diffuse"]=glm::vec3(1,0,0);
    ensi::gl::Program prog("texture.v.glsl", "texture.f.glsl");
    mat.program=prog;
    scene.materials["dice"]=mat;
    /*}}}*/
    /* Checkerboard material {{{*/
    ensi::gl::GLSLMaterial cbMat;
    cbMat.uniformSamplers["colormap"]=loadTGATexture("checkerboard.tga");
    cbMat.uniforms["diffuse"]=glm::vec3(1,1,0);
    cbMat.program=prog;
    scene.materials["checkerboard"]=cbMat;
    /*}}}*/
    /* Simple material {{{*/
    ensi::gl::GLSLMaterial simpleMat;
    prog=ensi::gl::Program("simple.v.glsl", "simple.f.glsl");
    simpleMat.program=prog;
    scene.materials["simple"]=simpleMat;
    /*}}}*/
    // bad cube 1/*{{{*/
    ensi::gl::Mesh badcube;
    makeACubeMappedCube(badcube,false);
    mw=controls.modelWorldMatrix.top();
    mw=glm::scale(mw, glm::vec3(2,2,2));
    mw=glm::translate(mw, glm::vec3(-1,0.5,0.5));
    scene.addObject("badcube", badcube, mw);/*}}}*/
    // bad cube 2/*{{{*/
    mw=controls.modelWorldMatrix.top();
    mw=glm::scale(mw, glm::vec3(2,2,2));
    mw=glm::translate(mw, glm::vec3(1,0.5,0.5));
    scene.addInstance("badcube",  mw, "badcube 2");/*}}}*/
    // cube/*{{{*/
    ensi::gl::Mesh cube;
    mw=controls.modelWorldMatrix.top();
    mw=glm::scale(mw, glm::vec3(2,2,2));
    mw=glm::translate(mw, glm::vec3(1,0.5,0.5));
    makeACubeMappedCube(cube,true);
    scene.addObject("cube", cube, mw);/*}}}*/
    // plane/*{{{*/
    ensi::gl::Mesh plane=badcube;
    mw=controls.modelWorldMatrix.top();
    mw=glm::scale(mw, glm::vec3(20,20,0.1));
    mw=glm::translate(mw, glm::vec3(0,0.5,0));
    for(ensi::gl::Vertex& v: plane.m_verts){
        v.uv=v.position.xy();
        v.uv*=20;
    }
    scene.addObject("plane", plane, mw);/*}}}*/
    /*}}}*/
    return 1;
}/*}}}*/
/*}}}*/


void draw()/*{{{*/
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT );
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    if(controls.exercise>=1){/*{{{*/
        GLuint textureId=scene.materials["dice"].uniformSamplers["dicetexture"];
        ensi::gl::Program prog=scene.materials["dice"].program;
        /*!todo exercise 1: set up the texture sampler for the dice program 
         *
         * Before doing anything else, don't forget first to fill in the code of
         * loadTGATexture in utils/texture.cpp (you should see a checkerboard
         * texture mapped on the floor)
         *
         *  1) Here you should activate the GL_TEXTURE_0 unit
         *  2) Then you should bind the texture location stored in
         * textureId on the GL_TEXTURE_2D target to this texture unit
         *  3) And last you should set up the uniform named "colormap" of the
         * shader program stored in prog accordingly to the chosen texture unit
         * 
         * Once all this is done, the two cubes should be mapped with textures
         * of a dice pattern (with digits from 1 to 6)
         * */
    }/*}}}*/
    if(controls.exercise>=1)/*{{{*/
    {
        // Nothing to be done here
        scene.drawObject("badcube","dice");
        if(controls.exercise>=2)
            scene.drawObject("cube","dice");
        else
            scene.drawObject("badcube 2","dice");
    }
    else
    {
        scene.drawObject("badcube","simple");
        scene.drawObject("cube","simple");
    }/*}}}*/
    GLuint checkerboardTextureLoc=scene.materials["checkerboard"].uniformSamplers["colormap"];
    if(controls.exercise>=3)/*{{{*/
    {
        /*!todo exercise 3: generate mipmaps and activate them for the min filter*/
        if(controls.exercise>=4)
        {
            /*!todo exercise 4: set up anisotropic filtering (using GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT)*/
        }
        else
        {
            /*!todo exercise 4: disable anisotropic filtering*/
        }

    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, checkerboardTextureLoc);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    }/*}}}*/
    if(controls.exercise>=1)
        scene.drawObject("plane","checkerboard");
    else
        scene.drawObject("plane","simple" );
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}
/*}}}*/

void makeACubeMappedCube(ensi::gl::Mesh& cube, bool correct)/*{{{*/
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
    std::vector<glm::vec2> uvs={
        glm::vec2(0,0), glm::vec2(0,1),  glm::vec2(1,1), glm::vec2(1,0),
        glm::vec2(0,0), glm::vec2(0,1),  glm::vec2(1,1), glm::vec2(1,0),
        //
        glm::vec2(0,0), glm::vec2(0,1),  glm::vec2(1,1), glm::vec2(1,0),
        glm::vec2(0,0), glm::vec2(0,1),  glm::vec2(1,1), glm::vec2(1,0),
        //
        glm::vec2(0,0), glm::vec2(0,1),  glm::vec2(1,1), glm::vec2(1,0),
        glm::vec2(0,0), glm::vec2(0,1),  glm::vec2(1,1), glm::vec2(1,0),
    };
    /*!todo exercise 2: Create the correct uv values to have each number mapped on one of the faces *//*{{{*/
    if(correct)
    {
        uvs={ };                         
    }
    /*}}}*/
    for (int i = 0; i < 24; ++i)
    {
        cube.m_verts.push_back(Vertex(positions[i], glm::vec3(), uvs[i]));
    }
    std::vector<GLuint> ibo=
    {
        0,2,1, 0,3,2,
        4,5,6, 4,6,7,
        8,10,9, 8,11,10,
        12,13,14, 12,14,15,
        16,17,18, 16,18,19,
        20,22,21, 20,23,22
    };
    cube.m_tris=ibo;
    cube.computeBSphere();
}
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
