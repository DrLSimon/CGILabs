#include <random>
#include "rubik.hpp"

//------------------- RubikGame
RubikGame::RubikGame ():controls(1)
{ 
    initGLContext();
    menuState = std::shared_ptr<IGameState>(new MenuState(*this));
    menu();
    int width;
    int height;
    GLFWwindow* window=controls.getWindow();
    glfwGetFramebufferSize(window, &width, &height);
    controls.cameraHandler.position=glm::vec3(0,-5,0);
}

RubikGame::~RubikGame ()
{
}

void RubikGame::run()
{
    GLFWwindow* window=controls.getWindow();
    // this just loops as long as the program runs
    while(!glfwWindowShouldClose(window))
    {
        // escape to quit, arrow keys to rotate view
        if (glfwGetKey(window, 'Q') == GLFW_PRESS)
            break;
        while((glfwGetTime()-controls.time) < 1./60)
        {
        }
        currentState->update();
        currentState->render();
    }
}

void RubikGame::menu()
{
    setState(menuState);
    controls.modelWorldMatrix.top()=glm::mat4();
}

void RubikGame::play()
{
    setState(std::shared_ptr<IGameState>(new PlayingState(*this, 15)));
}


void RubikGame::setState(std::shared_ptr<IGameState> state)
{
    currentState=state;
    state->defaultGLState();
    setCallbacks();
    int width;
    int height;
    GLFWwindow* window=controls.getWindow();
    glfwGetFramebufferSize(window, &width, &height);
    currentState->resize(window, width, height);
}

ensi::gl::Controls& RubikGame::getControls()
{
    return controls;
}

void RubikGame::defaultCubePose()
{
    
    glm::mat4& mw= controls.modelWorldMatrix.top();
    mw=glm::mat4();
    mw=glm::rotate(mw, glm::pi<float>()/4, {0,0,1});
    mw=glm::rotate(mw, glm::pi<float>()/4, glm::transpose(glm::mat3(mw))*glm::vec3(1,0,0));
}

void RubikGame::initGLContext()
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    ensi::gl::Renderer* renderer=new ensi::gl::GLRenderer();
    scene.setRenderer(renderer);
    scene.setControls(&controls);
    initGLFW();
    initGLEW();
    createGLScene();
}

void RubikGame::initGLFW()
{
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
    glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
    GLFWwindow* window=glfwCreateWindow(window_width, window_height, "A simple OpenGL Rubik's Cube", NULL, NULL);
    controls.setWindow(window);
    if (!window)
    {
        std::cerr<<"Could not open a window"<<std::endl;
        shutDown(1);
    }
    glfwMakeContextCurrent(window);
}

void RubikGame::initGLEW()
{
    // Do not forget to use glewExperimental so that glewInit works with core
    // profile: http://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();
    std::cout<<"Here, we should expect to get a GL_INVALID_ENUM (that's a known bug), and indeed:"<<std::endl;
    checkGLerror();
    if (GlewInitResult != GLEW_OK) {
        std::cerr<<"ERROR: "<< glewGetErrorString(GlewInitResult)<<std::endl;
        shutDown(1);
    }
}

void RubikGame::createGLScene()
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    // Create a material
    ensi::gl::GLSLMaterial material;
    ensi::gl::Program prog("rubik.v.glsl", "rubik.f.glsl");
    material.program=prog;
    material.uniforms["diffuse"]=glm::vec3(1,1,1);
    material.uniforms["exercise"]=(unsigned int)4;
    material.uniformSamplers["colormap"]=loadTGATexture("rubik.tga");
    std::string facenames[]={"white.tga", "red.tga", "green.tga", "orange.tga", "blue.tga", "yellow.tga"};
    material.samplerTargets["cubemap"]=GL_TEXTURE_CUBE_MAP;
    // Create the scene/*{{{*/
    ensi::gl::Mesh mesh;
    mesh.loadObj("yxz.obj", glm::vec4(1,1,1,1));
    int ky=-1;
    for( std::string y : {"n", "y", "f"})
    {
        facenames[0]=facenames[1]="black.tga";
        if(ky>0)
            facenames[0]="red.tga";
        else if(ky<0)
            facenames[1]="white.tga";
        int kx=-1;
        for( std::string x : {"l", "x", "r"})
        {
            facenames[2]=facenames[3]="black.tga";
            if(kx>0)
                facenames[2]="blue.tga";
            else if(kx<0)
                facenames[3]="yellow.tga";
            int kz=-1;
            for( std::string z : {"b", "z", "t"})
            {
                facenames[4]=facenames[5]="black.tga";
                if(kz>0)
                    facenames[4]="green.tga";
                else if(kz<0)
                    facenames[5]="orange.tga";
                std::string name= y + x + z;
                glm::mat4 mw;
                mw=glm::translate(mw, 2.f*glm::vec3(kx,ky,kz));
                scene.addObject(name, mesh, mw);
                GLenum facetypes[6]={GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
                material.uniformSamplers["cubemap"]=loadTGACubemap(facenames, facetypes);
                scene.materials[name]=material;
                kz++;
            }
            kx++;
        }
        ky++;
    }
    ensi::gl::Mesh cube;
    cube.makeMeCube();
    glm::mat4 mw;
    mw=glm::mat4();
    mw=glm::translate(mw, {0, 3, 0});
    mw=glm::scale(mw,{6,1,6});
    scene.materials["f"]=scene.materials["fxz"];
    scene.addObject("f", cube, mw);
    mw=glm::mat4();
    mw=glm::translate(mw, {0, -3, 0});
    mw=glm::scale(mw,{6,1.f,6});
    scene.materials["n"]=scene.materials["nxz"];
    scene.addObject("n", cube, mw);
    mw=glm::mat4();
    mw=glm::translate(mw, {3, 0, 0});
    mw=glm::scale(mw,{1,6,6});
    scene.materials["r"]=scene.materials["yrz"];
    scene.addObject("r", cube, mw);
    mw=glm::mat4();
    mw=glm::translate(mw, {-3, 0, 0});
    mw=glm::scale(mw,{1,6,6});
    scene.materials["l"]=scene.materials["ylz"];
    scene.addObject("l", cube, mw);
    mw=glm::mat4();
    mw=glm::translate(mw, {0, 0, 3});
    mw=glm::scale(mw,{6,6,1});
    scene.materials["t"]=scene.materials["yxt"];
    scene.addObject("t", cube, mw);
    mw=glm::mat4();
    mw=glm::translate(mw, {0, 0, -3});
    mw=glm::scale(mw,{6,6,1});
    scene.materials["b"]=scene.materials["yxb"];
    scene.addObject("b", cube, mw);
    /*}}}*/
}

void RubikGame::setCallbacks()
{
    using namespace std::placeholders;
    ResizeCallback rCallback;
    KeyCallback kCallback;
    MouseButtonCallback mbCallback;
    MouseMoveCallback mmCallback;
    rCallback= std::bind(&IGameState::resize, currentState, _1, _2, _3);
    kCallback=std::bind(&IGameState::keyCallback, currentState, _1, _2, _3, _4, _5);
    mbCallback= std::bind(&IGameState::buttonCallback, currentState, _1, _2, _3, _4);
    mmCallback=std::bind( &IGameState::moveCallback, currentState, _1, _2, _3);
    setResizeCallback(rCallback);
    setKeyCallback(kCallback);
    setButtonCallback(mbCallback);
    setMoveCallback(mmCallback);
}

void RubikGame::setResizeCallback(const RubikGame::ResizeCallback& callback)
{
    auto f=bind<ResizeId>(callback);
    GLFWwindow* window=controls.getWindow();
    glfwSetFramebufferSizeCallback(window, f);
}

void RubikGame::setKeyCallback(const RubikGame::KeyCallback& callback)
{
    
    auto f=bind<KeyId>(callback);
    GLFWwindow* window=controls.getWindow();
    glfwSetKeyCallback(window, f);
}

void RubikGame::setButtonCallback(const RubikGame::MouseButtonCallback& callback)
{
    auto f=bind<ButtonId>(callback);
    GLFWwindow* window=controls.getWindow();
    glfwSetMouseButtonCallback(window, f);
    
}

void RubikGame::setMoveCallback(const RubikGame::MouseMoveCallback& callback)
{
    auto f=bind<MoveId>(callback);
    GLFWwindow* window=controls.getWindow();
    glfwSetCursorPosCallback(window, f);
}

glm::vec2 RubikGame::getCursorPosition() const
{
    const ensi::gl::Camera& camera=controls.getCamera();
    double x, y;
    glfwGetCursorPos(controls.getWindow(), &x, &y);
    x*=camera.screenToPixel[0];
    y*=camera.screenToPixel[1];
    return {x, y};
}

void RubikGame::shutDown(int return_code)
{
    glfwTerminate();
    exit(return_code);
}


//------------------- PlayingState
PlayingState::PlayingState(RubikGame& g, unsigned int nbPerms):game(g), nbPermutation(nbPerms), playingState(GENERATE_PERMUTATION), rotatingSlice(' '),mx(0),my(0),angle(0), frame(0)
{
    for( std::string y : {"n", "y", "f"})
    {
        for( std::string x : {"l", "x", "r"})
        {
            for( std::string z : {"b", "z", "t"})
            {
                std::string name=y + x + z;
                rubikPiecesNames.insert(name);
            }
        }
    }
    rubikFaceNames={"n", "f", "l", "r", "t", "b"};
}

void PlayingState::defaultGLState()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1,1,1,1);
}

void PlayingState::update()
{
    double current_time = glfwGetTime();
    ensi::gl::Controls& controls=game.getControls();
    controls.setTime(current_time);
    controls.updateView();
    frame++;
    int nbTours=glm::round(angle/(glm::pi<double>()*2));
    angle-=nbTours*2*glm::pi<double>();
    if(playingState!=IDDLE and playingState!=WAITING_ROTATION)
    {
        cube.rotateSlice(rotatingSlice, angle, axis);
    }
    if(playingState==FINISHING_ROTATION)
    {
        int nbQuarters=glm::round(angle/glm::pi<double>()*2);
        if(nbQuarters<0)
        {
            nbQuarters+=4;
            angle+=2*glm::pi<double>();
        }
        double target=nbQuarters*glm::pi<double>()/2;
        bool needMore=target>angle;
        angle+=5*(target-angle)*(controls.time-controls.oldtime);
        float eps=1e-2;
        if((angle>(target-eps) and needMore) or (angle<(target+eps) and not needMore))
        {
            angle=0;
            playingState=IDDLE;
            cube.updatePermutation(rotatingSlice, nbQuarters);
            cube.currentSliceMatrix=glm::mat4();
        }
        return;
    }
    if(playingState==UPDATE_PERMUTATION)
    {
        int nbQuarters=1;
        double target=nbQuarters*glm::pi<double>()/2;
        bool needMore=target>angle;
        angle+=15*(target-angle)*(controls.time-controls.oldtime);
        float eps=1e-2;
        if((angle>(target-eps) and needMore) or (angle<(target+eps) and not needMore))
        {
            angle=0;
            if(nbPermutation>0)
            {
                nbPermutation--;
                playingState=GENERATE_PERMUTATION;
            }
            else
                playingState=IDDLE;
            cube.updatePermutation(rotatingSlice, nbQuarters);
            cube.currentSliceMatrix=glm::mat4();
        }
        return;
    }
    if(playingState==GENERATE_PERMUTATION)
    {
        std::vector<char> slices={'l', 'x', 'r', 'n', 'y', 'f', 't', 'z', 'b'};
        std::random_device rd;
        std::mt19937 g(rd());
        unsigned int pos=((unsigned int)std::rand()) % 9;
        rotatingSlice=slices[pos];
        angle=0;
        axis={0,0,0};
        axis[pos/3]=1;
        playingState=UPDATE_PERMUTATION;
        return;
    }
}

void PlayingState::render()
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    ensi::gl::Controls& controls=game.getControls();
    GLFWwindow* window=controls.getWindow();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for( std::string y : {"n", "y", "f"})
    for( std::string x : {"l", "x", "r"})
    for( std::string z : {"b", "z", "t"})
    {
        std::string name=y + x + z;
        std::string newname=cube.permutation.at(name);
        glm::mat4 mw=controls.modelWorldMatrix.top();
        if(newname[0]==rotatingSlice or newname[1]==rotatingSlice or newname[2]==rotatingSlice)
        {
            mw=mw*cube.currentSliceMatrix;
        }
        glm::mat4 permutMat=cube.permutation.rotationAt(name);
        mw=mw*permutMat;
        controls.modelWorldMatrix.push(mw);
        scene.drawObject(name,name);
        controls.modelWorldMatrix.pop();
    }
    checkGLerror();
    glfwSwapBuffers(window);
    glfwPollEvents();
    controls.resetView();
}

void PlayingState::keyCallback(GLFWwindow* window, int key, int scancode,  int action, int modifiers)/*{{{*/
{
    ensi::gl::Controls& controls=game.getControls();
    if(action==GLFW_PRESS)
    {
        switch(key)
        {
            case 'R':
                game.defaultCubePose();
                return;
            case GLFW_KEY_ESCAPE:
                game.menu();
                return;
        }
    }
    controls.keyCallback(key, action, modifiers);
}

void PlayingState::buttonCallback(GLFWwindow* window, int button, int action, int modifiers)/*{{{*/
{
    ensi::gl::Scene& scene=ensi::gl::Scene::getInstance();
    ensi::gl::Controls& controls=game.getControls();
    glm::vec2 cursor=game.getCursorPosition();
    if(button==GLFW_MOUSE_BUTTON_LEFT)
    {
        if(action==GLFW_PRESS and playingState==IDDLE){
            mx=cursor.x;
            my=cursor.y;
            controls.picker.pickableNames=rubikFaceNames;
            pickedface=controls.picker.performPicking(scene, mx,  my);
            controls.picker.pickableNames=rubikPiecesNames;
            pickedpiece=controls.picker.performPicking(scene, mx,  my);
            std::cout << "Face " << pickedface << " Piece " << pickedpiece << std::endl;
            if(pickedface!="" and pickedpiece!="")
            {
                playingState=WAITING_ROTATION;            }
            else{
                playingState=ROTATING_WORLD;
            }
        }
        else if(action==GLFW_RELEASE)
        {
            if(playingState==ROTATING_SLICE)
                playingState=FINISHING_ROTATION;
            else if(playingState==ROTATING_WORLD)
                playingState=IDDLE;
        }
    }
}

glm::vec3 PlayingState::mouseMoveInWorldSPace(const glm::vec2& cursor) const
{
    ensi::gl::Controls& controls=game.getControls();
    const glm::vec4& viewport=controls.getViewport();
    const glm::mat4& mw=controls.getWorldMatrix();
    const glm::mat4& wv=controls.getViewMatrix();
    const glm::mat4& proj=controls.getProjectionMatrix();
    float height=viewport[3];
    glm::vec3 deltaPos=glm::unProject({cursor.x, height-1-cursor.y,0}, wv*mw, proj, viewport) -glm::unProject({mx,height-1-my,0}, wv*mw, proj, viewport);
    return deltaPos;
}

void PlayingState::moveCallback(GLFWwindow* window, double xx, double yy)
{
    ensi::gl::Controls& controls=game.getControls();
    glm::vec2 cursor=game.getCursorPosition();
    glm::vec2 deltaPixels= cursor- glm::vec2(mx,my);
    float sensitivity=0.005;
    if(playingState==ROTATING_WORLD)
    {
        mx=cursor.x;
        my=cursor.y;
        glm::mat4& mw= controls.modelWorldMatrix.top();
        mw=glm::rotate(mw, sensitivity*2*glm::pi<float>()*deltaPixels.y, glm::transpose(glm::mat3(mw))*glm::vec3(1,0,0));
        mw=glm::rotate(mw, sensitivity*2*glm::pi<float>()*deltaPixels.x, glm::transpose(glm::mat3(mw))*glm::vec3(0,0,1));
        return;
    }
    glm::vec3 deltaPos=mouseMoveInWorldSPace(cursor);
    if(glm::length(deltaPixels)>20 and playingState==WAITING_ROTATION)
    {
        playingState=ROTATING_SLICE;
        std::swap(pickedpiece[0], pickedpiece[1]); //!fixme: this is a side effect of the y+x+z convention (better use x+y+z instead)
        int pos=pickedpiece.find(pickedface[0]);
        if(pos==std::string::npos)
            return;
        glm::vec3 faceNormal={0,0,0};
        glm::vec3 faceTangent={0,0,0};
        glm::vec3 faceBitangent={0,0,0};
        faceNormal[pos]=1;
        faceTangent[(pos+1)%3]=1;
        faceBitangent[(pos+2)%3]=1;
        if(glm::abs(glm::dot(deltaPos, faceTangent))>glm::abs(glm::dot(deltaPos, faceBitangent)))
        {
            rotatingSlice=pickedpiece[(pos+2)%3];
            axis=faceBitangent;
            biaxis=faceTangent;
        }
        else
        {
            rotatingSlice=pickedpiece[(pos+1)%3];
            axis=faceTangent;
            biaxis=-faceBitangent;
        }
        std::cout << "rotating slice " << rotatingSlice << std::endl;
    }
    if(playingState==ROTATING_SLICE)
    {
        angle = (glm::dot(deltaPos, biaxis)) * 10*glm::pi<float>();
        if(pickedface=="l" or pickedface=="n" or pickedface=="b")
            angle=-angle;
    }
}

void PlayingState::resize(GLFWwindow* window, int width, int height)
{
    ensi::gl::Controls& controls = game.getControls();
    controls.resize(width, height);
}


//------------------- MenuState
MenuState::MenuState(RubikGame& g): game(g)
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    // Create a material
    ensi::gl::GLSLMaterial material;
    ensi::gl::Program prog("menu.v.glsl", "menu.f.glsl");
    material.program=prog;
    iddleMenuTexture=loadTGATexture("menuIddle.tga");
    hoverMenuTexture=loadTGATexture("menuHover.tga");
    material.uniformSamplers["colormap"]=iddleMenuTexture;
    scene.materials["menu"]=material;
    menuMask = read_tga("menuMask.tga", w, h, d);
    // Mesh
    ensi::gl::Mesh plane;
    plane.makeMePlane();
    glm::mat4 mw;
    mw=glm::rotate(mw, -glm::pi<float>()/2, {1,0,0});
    scene.addObject("menu", plane, mw);
    // testing with digits
    ensi::gl::Program progdigits("digits.v.glsl", "digits.f.glsl");
    material.program=progdigits;
    material.uniformSamplers["colormap"]=loadTGATexture("digits.tga");
    float vShift=0.5;
    for (int i=0 ; i<10; i++) {
        if (i==5) {
            vShift=0.;
        }
        material.uniforms["uvShift"]=glm::vec2(0.2*i,vShift);
        std::string matname="digit"+std::to_string(i);
        std::cout << "the digit " << matname << std::endl;
        scene.materials[matname]=material;
    }
    mw={};
    float s=0.1;
    mw=glm::scale(mw, {s,s,1});
    mw=glm::rotate(mw, -glm::pi<float>()/2, {1,0,0});
    mw=glm::translate(mw, {0,-1,0});
    scene.addObject("digit", plane, mw);
}

MenuState::~MenuState()
{
    delete [] menuMask;
    glDeleteTextures(1, &iddleMenuTexture);
    glDeleteTextures(1, &hoverMenuTexture);
}

void MenuState::update()
{
}

void MenuState::defaultGLState()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2, 0.2, 0.2, 1);
}

void MenuState::render()
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    ensi::gl::Controls& controls=game.getControls();
    GLFWwindow* window=controls.getWindow();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.drawObject("menu", "menu");
    glm::mat4 mw=controls.modelWorldMatrix.top();
    mw=glm::translate(mw, {-1,0.3,0});
    for(int i=0; i< 10; i++)
    {
        mw=glm::translate(mw, {0.2,0,0});
        controls.modelWorldMatrix.push(mw);
        scene.drawObject("digit", "digit" +std::to_string(i));
        controls.modelWorldMatrix.pop();
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void MenuState::keyCallback(GLFWwindow* window, int key, int scancode,  int action, int modifiers)
{
    std::cout << "I don't care about your keys!" << std::endl;
}

void MenuState::buttonCallback(GLFWwindow* window, int button, int action, int modifiers)
{
    if(button==GLFW_MOUSE_BUTTON_LEFT and action==GLFW_PRESS)
    {
        glm::vec2 cursor=game.getCursorPosition();
        if (maskAt(cursor.x, cursor.y)==10)
        {
            game.defaultCubePose();
            game.play();
            std::cout<< "GO HAVE FUN!!!!!!" << std::endl;
        }
        else
        {
            std::cout<< "Not on the play button. Try again!" << std::endl;
        }
    }
}

int MenuState::maskAt(float x, float y)
{
    float x0=maskViewport[0];
    float y0=maskViewport[1];
    int j=maskRatio*(x-x0);
    int i=maskRatio*(y-y0);
    if(i<0 or i>=h or j<0 or j>=w)
        return -1;
    else
        return menuMask[i*w*d +j*d];
}

void MenuState::moveCallback(GLFWwindow* window, double x, double y)
{
    ensi::gl::Scene& scene = ensi::gl::Scene::getInstance();
    ensi::gl::GLSLMaterial& material = scene.materials["menu"];
    glm::vec2 cursor=game.getCursorPosition();
    if (maskAt(cursor.x, cursor.y)==10)
    {
        material.uniformSamplers["colormap"]=hoverMenuTexture;
    }
    else
    {
        material.uniformSamplers["colormap"]=iddleMenuTexture;
    }
}

void MenuState::resize(GLFWwindow* window, int width, int height)
{
    ensi::gl::Controls& controls=game.getControls();
    controls.resize(width, height);
    float extension=std::min(width, height);
    float wRatio=width/extension;
    float hRatio=height/extension;
    maskRatio=518./extension;
    controls.cameraHandler.camera.projectionMatrix=glm::ortho(-wRatio, wRatio, -hRatio, hRatio, -1.f , 1.f);
    const glm::vec4& viewport=controls.getViewport();
    maskViewport[0]=(viewport[0]+viewport[2])/2. - (viewport[2]-viewport[0])/2./wRatio;
    maskViewport[1]=(viewport[1]+viewport[3])/2. - (viewport[3]-viewport[1])/2./hRatio;
    maskViewport[2]=(viewport[0]+viewport[2])/2. + (viewport[2]-viewport[0])/2./wRatio;
    maskViewport[3]=(viewport[1]+viewport[3])/2. + (viewport[3]-viewport[1])/2./hRatio;
}


//------------------- RubikCube
RubikCube::RubikCube()
{
}

RubikCube::~RubikCube()
{
}

void RubikCube::rotateSlice(char slice, float angle, const glm::vec3& axis)
{
    currentSliceMatrix=glm::rotate(glm::mat4(), angle, axis);
}

void RubikCube::updatePermutation(char slice, int nbQuarters)/*{{{*/
{
    Permutation newPermutation;
    std::vector<std::string> oldNames;
    std::string s=std::string(1, slice);
    switch (slice)
    {
        case 'n':
        case 'y':
        case 'f':
            oldNames={s+"lb",s+"lz",s+"lt", s+"xt", s+"rt", s+"rz", s+"rb", s+"xb", s+"xz"};
            break;
        case 'l':
        case 'x':
        case 'r':
            oldNames={"n"+s+"t", "n"+s+"z", "n"+s+"b", "y"+s+"b", "f"+s+"b", "f"+s+"z","f"+s+"t","y"+s+"t", "y"+s+"z"};
            break;
        case 'b':
        case 'z':
        case 't':
            oldNames={"nl"+s,"nx"+s,"nr"+s, "yr"+s, "fr"+s, "fx"+s, "fl"+s, "yl"+s, "yx"+s};
            break;
        default:
            return;
    }
    newPermutation.sliceRotation(oldNames, nbQuarters,  currentSliceMatrix);
    permutation.rightComposeWith(newPermutation);
}


//--------------------- Permutation class
Permutation::Permutation()
{
    for( std::string y : {"n", "y", "f"})
    for( std::string x : {"l", "x", "r"})
    for( std::string z : {"b", "z", "t"})
    {
        std::string name=y + x + z;
        mapping[name]=name;
        rotations[name]=glm::imat4();
    }
    
}

void Permutation::rightComposeWith(const Permutation& rhs)
{
    for( auto& entry : mapping)
    {
        std::string name=entry.first;
        std::string currentname=entry.second;
        mapping[name]=rhs.at(currentname);
        if(rhs.at(currentname)!=currentname)
        {
            auto& rotation=rotations[name];
            const auto& other=rhs.rotations.at(currentname);
            rotation=other*rotation;
        }
    }
}

void Permutation::sliceRotation(const std::vector<std::string>& oldnames, unsigned int nbQuarters,  const glm::mat4& rotation)
{
    glm::imat4 irot(glm::round(rotation[0]),glm::round(rotation[1]),glm::round(rotation[2]),glm::round(rotation[3]));
    for(unsigned int k=0; k<8; k++)
    {
        mapping[oldnames[k]]=oldnames[(k+2*nbQuarters)%8];
        rotations[oldnames[k]]=irot;
    }
    rotations[oldnames[8]]=irot;
}

void Permutation::printMe() const
{
    for( auto& entry : mapping)
    {
        if(entry.first!=entry.second)
            std::cout << entry.first << " -> " << entry.second << std::endl;
    }
}

std::string& Permutation::operator[](const std::string& input)
{
    return mapping[input];
}

const std::string& Permutation::at(const std::string& input) const
{
    return mapping.at(input);
}

const glm::imat4& Permutation::rotationAt(const std::string& input) const
{
    return rotations.at(input);
}

