include(FetchContent)

FetchContent_Declare(Catch2
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG v3.13.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_executable(pointer_registry_test
    "pointer_registry_test.cc"
    "${PROJECT_SOURCE_DIR}/src/pointer_registry.cc"
)

target_include_directories(pointer_registry_test PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

target_link_libraries(pointer_registry_test PRIVATE
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(pointer_registry_test)
