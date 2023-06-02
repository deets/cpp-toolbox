cmake_minimum_required(VERSION 3.26)

project("Timed Finite Automaton" LANGUAGES CXX VERSION 1.0.0)

find_package(Catch2 3 REQUIRED)


include_directories("include")

# These tests can use the Catch2-provided main
add_executable(test tests/test.cpp)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
target_compile_features(test PRIVATE cxx_std_17)
