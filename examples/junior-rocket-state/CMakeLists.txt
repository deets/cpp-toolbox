cmake_minimum_required(VERSION 3.26)

project("Junior Rocket State" LANGUAGES CXX VERSION 1.0.0)


add_executable(
  junior-rocket-state
  main.cpp
  junior-rocket-state.cpp
  simulator.cpp
)

include_directories(third-party/eigen-3.4.0)

target_compile_definitions(junior-rocket-state
  PRIVATE
  USE_IOSTREAM
)

target_compile_features(junior-rocket-state PUBLIC cxx_std_17)
target_compile_options(junior-rocket-state PRIVATE -Wall -Wextra -Wpedantic -Werror)


# These tests can use the Catch2-provided main
find_package(Catch2 3 REQUIRED)

add_executable(
  junior-rocket-state-tests
  parabola-fitting-tests.cpp
)

target_link_libraries(junior-rocket-state-tests PRIVATE Catch2::Catch2WithMain)
target_compile_features(junior-rocket-state-tests PUBLIC cxx_std_17)
