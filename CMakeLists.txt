cmake_minimum_required(VERSION 3.10)
project(LifeGame)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)
set(SDL_TESTS OFF)

add_executable(LifeGame LifeGame.cpp)

add_subdirectory(SDL)

target_link_libraries(LifeGame SDL3::SDL3)