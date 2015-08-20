
### Creating an executable
add_executable(shadows shadows.cpp)
target_link_libraries(shadows utils ${GL_LIBRARIES} ${APPLE_FRAMEWORKS} ${GLFW3_STATIC_LIBRARIES} ${ASSIMP_LIBRARIES})
add_dependencies(shadows resources)
