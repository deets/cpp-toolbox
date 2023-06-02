cmake_minimum_required(VERSION 3.26)

project("Junior Rocket State" LANGUAGES CXX VERSION 1.0.0)


add_executable(junior-rocket-state junior-rocket-state.cpp)
target_compile_features(junior-rocket-state PUBLIC cxx_std_17)
