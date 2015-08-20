set(RUBIKGAMESRC RubikCube/rubik.cpp RubikCube/rubik.hpp)

### Creating an executable
add_executable(rubik RubikCube/game.cpp ${RUBIKGAMESRC})
target_link_libraries(rubik utils ${GL_LIBRARIES} ${APPLE_FRAMEWORKS} ${GLFW3_STATIC_LIBRARIES} ${ASSIMP_LIBRARIES})
add_dependencies(rubik rubikresources)

