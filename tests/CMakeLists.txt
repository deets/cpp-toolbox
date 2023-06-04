# These tests can use the Catch2-provided main
find_package(Catch2 3 REQUIRED)

add_executable(
  tests
  statistics-tests.cpp
  automaton-tests.cpp
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_compile_features(tests PUBLIC cxx_std_17)
