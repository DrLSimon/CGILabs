cmake_minimum_required(VERSION 2.6)

project(CGILabs)
enable_testing()

###  Settings  ###{{{
set(CMAKE_CXX_FLAGS "-std=c++11 -DGL_GLEXT_PROTOTYPES -I/opt/X11/include") # Don't forget to define GL_GLEXT_PROTOTYPES to ensure that the glCreateShader and other prototypes are declared. Note we could do it in the main file via "#define GL_GLEXT_PROTOTYPES" before "#include <GL/glext.h>"
## setting build type to debug
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_BUILD_TYPE DEBUG)
LINK_DIRECTORIES("~/local_install/lib")
LINK_DIRECTORIES("/usr/local/lib")
INCLUDE_DIRECTORIES("/usr/local/include")
INCLUDE_DIRECTORIES("~/local_install/include")
INCLUDE_DIRECTORIES("./3rd-parties/")
###  Settings  ###}}}

###  OpenGL  ###{{{
find_package(OpenGL)
if (OPENGL_FOUND)
   include_directories(${OPENGL_INCLUDE_DIR})
else (OPENGL_FOUND)
    message("OpenGL environment missing")
endif (OPENGL_FOUND)
set(GL_LIBRARIES ${OPENGL_LIBRARIES} GLEW)
if(APPLE)
 set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}")
 SET(APPLE_FRAMEWORKS "-framework Cocoa -framework IOkit -framework CoreVideo")
endif(APPLE)
###  OpenGL  ###}}}

###  GLFW3  ###{{{
set( ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:$ENV{HOME}/local_install/lib/pkgconfig")
FIND_PACKAGE( PkgConfig REQUIRED )
PKG_SEARCH_MODULE( GLFW3 REQUIRED glfw3 ) # sets GLFW3 as prefix for glfw vars
# now ${GLFW3_INCLUDE_DIR}, ${GLFW3_LIBRARIES} and ${GLFW3_STATIC_LIBRARIES} 
# are set
INCLUDE_DIRECTORIES( ${GLFW3_INCLUDE_DIR} )
###  GLFW3  ###}}}

###  ASSIMP  ###{{{
PKG_SEARCH_MODULE( ASSIMP REQUIRED assimp ) 
include_directories(${ASSIMP_INCLUDE_DIRS})
link_directories(${ASSIMP_LIBRARY_DIRS})
###  ASSIMP  ###}}}

###  internal util library  ###{{{
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(UTILS_SRC include/lightGlAPI.hpp include/lightGlAPI.cpp utils/utils.cpp utils/shaders.cpp utils/vao.cpp utils/textures.cpp
  utils/objloader.cpp utils/tangentspace.cpp utils/vboindexer.cpp)
add_library(utils ${UTILS_SRC})

###  internal util library  ###}}}


###  Copying resources (shaders, images,...)  ###{{{
set(copycmd create_symlink)
if(WIN32)
  set(copycmd copy_if_different)
endif(WIN32)

macro(copy_files GLOBPAT DESTINATION THETARGET)
  file(GLOB COPY_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLOBPAT})
  set(COPIED_FILES)
  foreach(FILENAME ${COPY_FILES})
    get_filename_component(BASENAME ${FILENAME} NAME)
    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}"
      COMMAND ${CMAKE_COMMAND} -E ${copycmd}
      "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}"
      "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}"
      DEPENDS
      "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}"
      COMMENT
      "*** COPYING: ${FILENAME} to ${BASENAME}"
      )
    list(APPEND COPIED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}")
  endforeach(FILENAME)
  add_custom_target(${THETARGET} ALL DEPENDS ${COPIED_FILES})
endmacro(copy_files)
###  Copying resources (shaders, images,...)  ###}}}

### Copying resources in the runtime directory (images, shaders,...) ###{{{
copy_files("resources/*.tga" ${CMAKE_BINARY_DIR} tgafiles)
copy_files("resources/*.obj" ${CMAKE_BINARY_DIR} objfiles)
copy_files("resources/*.glsl" ${CMAKE_BINARY_DIR} glslfiles)
add_custom_target(resources ALL DEPENDS tgafiles objfiles glslfiles)
###}}}

###  Sub projects  ###{{{
# 2A
add_subdirectory(Minimal)
add_subdirectory(Textures)
add_subdirectory(Shading)
# 3A
add_subdirectory(Mirror)
add_subdirectory(Vshadows)
# Game Example
add_subdirectory(Game)
copy_files("Game/RubikCube/resources/*.tga" ${CMAKE_BINARY_DIR} rubiktgafiles)
copy_files("Game/RubikCube/resources/*.glsl" ${CMAKE_BINARY_DIR} rubikglslfiles)
copy_files("Game/RubikCube/resources/*.obj" ${CMAKE_BINARY_DIR} rubikobjfiles)
add_custom_target(rubikresources ALL DEPENDS rubiktgafiles rubikobjfiles rubikglslfiles)
###  Sub projects  ###}}}



# vim: set foldmethod=marker:
