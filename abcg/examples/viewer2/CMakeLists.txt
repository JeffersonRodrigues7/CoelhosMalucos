project(viewer2)
add_executable(${PROJECT_NAME} main.cpp model.cpp window.cpp trackball.cpp camera.cpp)
enable_abcg(${PROJECT_NAME})
