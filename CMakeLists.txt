cmake_minimum_required(VERSION 3.5.0)
project(flair)

include_directories(ext/include)
add_executable(flair main.c)
target_link_libraries(flair GL GLEW SDL2)