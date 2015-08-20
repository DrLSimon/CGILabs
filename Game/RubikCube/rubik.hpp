#ifndef RUBIK_HPP
#define RUBIK_HPP
#include <stdlib.h>
#include <iostream>
#include <stack>
#include <vector>
#include <functional>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
// matrix and vectors
// to declare before including glm.hpp, to use the swizzle operators
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_integer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include "../include/lightGlAPI.hpp"


class Permutation {
public:
    Permutation();
    void rightComposeWith(const Permutation& rhs);
    void printMe() const;
    std::string& operator[](const std::string& input);
    const std::string& at(const std::string& input) const;
    const glm::imat4& rotationAt(const std::string& input) const;
    void sliceRotation(const std::vector<std::string>& oldnames, unsigned int nbQuarters,  const glm::mat4& rotation);
    
private:
    std::map<std::string, std::string> mapping;
    std::map<std::string, glm::imat4> rotations;
};

class RubikCube
{
public:
    RubikCube ();
    virtual ~RubikCube ();
    void rotateSlice(char slice, float angle, const glm::vec3& axis);
    void updatePermutation(char slice, int nbQuarters);
    
public:
    Permutation permutation;
    glm::mat4 currentSliceMatrix;
};

class IGameState
{
public:
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void defaultGLState() =0;
    virtual void keyCallback(GLFWwindow* window, int key, int scancode,  int action, int modifiers) = 0;
    virtual void buttonCallback(GLFWwindow* window, int button, int action, int modifiers) = 0;
    virtual void moveCallback(GLFWwindow* window, double x, double y) = 0;
    virtual void resize(GLFWwindow* window, int width, int height) = 0;
};


class RubikGame
{
public: // Constructor and main loop
    RubikGame ();
    virtual ~RubikGame ();
    void run();
    
public: // Utilitary routines
    void defaultCubePose();
    glm::vec2 getCursorPosition() const;
    void shutDown(int return_code);
    void menu();
    void play();
    ensi::gl::Controls& getControls();

private: // Callbacks wrappers
    typedef std::function<void(GLFWwindow*, int, int)> ResizeCallback;
    typedef std::function<void(GLFWwindow*, int, int, int ,int)> KeyCallback;
    typedef std::function<void(GLFWwindow*, int, int, int)> MouseButtonCallback;
    typedef std::function<void(GLFWwindow*, double, double)> MouseMoveCallback;
    enum CallbackID {ResizeId=0, KeyId=1, ButtonId=2, MoveId=3};
    void setResizeCallback(const ResizeCallback& callback);
    void setKeyCallback(const KeyCallback& callback);
    void setButtonCallback(const MouseButtonCallback& callback);
    void setMoveCallback(const MouseMoveCallback& callback);
    void setCallbacks();

private: // Context and scene creation
    void initGLContext();
    void initGLFW();
    void initGLEW();
    void createGLScene();
    void setState(std::shared_ptr<IGameState> state);

private: // Fields
    ensi::gl::Controls controls;
    std::shared_ptr<IGameState> currentState;
    std::shared_ptr<IGameState> menuState;
};

class PlayingState: public IGameState
{
public:
    PlayingState(RubikGame& g, unsigned int nbPerms);
    void update();
    void render();
    void defaultGLState();
    void keyCallback(GLFWwindow* window, int key, int scancode,  int action, int modifiers);
    void buttonCallback(GLFWwindow* window, int button, int action, int modifiers);
    void moveCallback(GLFWwindow* window, double x, double y);
    void resize(GLFWwindow* window, int width, int height);
    
    glm::vec3 mouseMoveInWorldSPace(const glm::vec2& cursor) const;
private:
    RubikGame& game;
    unsigned int nbPermutation;
    enum {GENERATE_PERMUTATION, UPDATE_PERMUTATION, IDDLE, WAITING_ROTATION, ROTATING_SLICE, FINISHING_ROTATION, ROTATING_WORLD} playingState;
    RubikCube cube;
    char rotatingSlice;
    std::string pickedpiece;
    std::string pickedface;
    double mx;
    double my;
    double angle;
    glm::vec3 axis;
    glm::vec3 biaxis;
    std::set<std::string> rubikPiecesNames;
    std::set<std::string> rubikFaceNames;
    long int frame;
};

class MenuState: public IGameState
{
public:
    MenuState(RubikGame& g);
    ~MenuState();
    void update();
    void render() ;
    void defaultGLState();
    void keyCallback(GLFWwindow* window, int key, int scancode,  int action, int modifiers);
    void buttonCallback(GLFWwindow* window, int button, int action, int modifiers);
    void moveCallback(GLFWwindow* window, double x, double y);
    void resize(GLFWwindow* window, int width, int height);
    
    int maskAt(float x, float y);
    
private:
    RubikGame& game;
    GLuint iddleMenuTexture;
    GLuint hoverMenuTexture;
    int w, h, d;
    char* menuMask;
    glm::vec4 maskViewport;
    float maskRatio;
};


template <const size_t theId, typename R, typename... Args>
class FunctionPointer
{
public:
    static R invoke(Args... args)
    {
        return instance().theFunction(args...);
    }
    
    
    typedef std::function<R(Args...)> FunctionType;
    typedef decltype(&FunctionPointer::invoke) FunctionPointerType;
    
    
    static FunctionPointerType bind(FunctionType&& f)
    {
        instance().theFunction.swap(f);
        return ptr();
    }
    
    static FunctionPointerType bind(const FunctionType& f)
    {
        instance().theFunction=f;
        return ptr();
    }
    
    static FunctionPointerType ptr()
    {
        return &invoke;
    }
    
private:
    static FunctionPointer& instance()
    {
        static FunctionPointer theInstance;
        return theInstance;
    }
    
    FunctionPointer() {}
    
    FunctionType theFunction;
};

template <const size_t theId, typename R, typename... Args>
typename FunctionPointer<theId, R, Args...>::FunctionPointerType
bind(const std::function<R(Args...)>& f)
{
    
    return FunctionPointer<theId, R, Args...>::bind(f);
}
#endif /* end of include guard: RUBIK_HPP */
