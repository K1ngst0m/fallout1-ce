cmake_minimum_required(VERSION 3.13)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project(fallout-ce)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

option(ASAN "Enable address sanitizer" OFF)
option(UBSAN "Enable undefined behaviour sanitizer" OFF)
option(BUILD_TESTING "Build tests" OFF)

include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_executable(fallout-ce)

if(NOT EMSCRIPTEN)
    if(ASAN)
        target_compile_options(fallout-ce PUBLIC "-fsanitize=address;-fsanitize-recover=address")
        target_link_options(fallout-ce PUBLIC "-fsanitize=address;-fsanitize-recover=address")
    endif()
    if(UBSAN)
        target_compile_options(fallout-ce PUBLIC "-fsanitize=undefined")
        target_link_options(fallout-ce PUBLIC "-fsanitize=undefined")
    endif()
endif()

target_include_directories(fallout-ce PUBLIC src)

add_subdirectory(src)

add_subdirectory("third_party/adecode")
target_link_libraries(fallout-ce adecode::adecode)

add_subdirectory("third_party/fpattern")
target_link_libraries(fallout-ce fpattern::fpattern)

if(EMSCRIPTEN)
    include(cmake/platform/Emscripten.cmake)
else()
    find_package(SDL3 3.2.0 REQUIRED)
    target_link_libraries(fallout-ce SDL3::SDL3)
endif()

if(BUILD_TESTING AND NOT EMSCRIPTEN)
    add_subdirectory("tests")
endif()
