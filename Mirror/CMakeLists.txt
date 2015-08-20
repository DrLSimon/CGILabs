
### Creating an executable
add_executable(mirror mirror.cpp)
target_link_libraries(mirror utils ${GL_LIBRARIES} ${APPLE_FRAMEWORKS} ${GLFW3_STATIC_LIBRARIES} ${ASSIMP_LIBRARIES})
add_dependencies(mirror resources)
