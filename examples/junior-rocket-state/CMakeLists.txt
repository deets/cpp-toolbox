cmake_minimum_required(VERSION 3.26)

project("Junior Rocket State" LANGUAGES CXX VERSION 1.0.0)


add_executable(
  junior-rocket-state
  main.cpp
  junior-rocket-state.cpp
  simulator.cpp
)

target_compile_features(junior-rocket-state PUBLIC cxx_std_17)
target_compile_options(junior-rocket-state PRIVATE -Wall -Wextra -Wpedantic -Werror)
