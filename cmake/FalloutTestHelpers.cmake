# Helper function to reduce per-test boilerplate in tests/CMakeLists.txt.
#
# Usage:
#   fallout_add_test(my_test
#       SOURCES my_test.cc "${PROJECT_SOURCE_DIR}/src/some_module.cc"
#       LIBS SDL3::SDL3 fpattern::fpattern
#   )

function(fallout_add_test name)
    cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
    add_executable(${name} ${ARG_SOURCES})
    target_include_directories(${name} PRIVATE "${PROJECT_SOURCE_DIR}/src")
    target_link_libraries(${name} PRIVATE Catch2::Catch2WithMain ${ARG_LIBS})
    catch_discover_tests(${name} TEST_PREFIX "${name}::")
endfunction()
