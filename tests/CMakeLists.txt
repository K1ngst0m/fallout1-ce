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

add_executable(lifecycle_test
    "lifecycle_test.cc"
    "${PROJECT_SOURCE_DIR}/src/plib/gnw/lifecycle.cc"
)

add_executable(input_runtime_test
    "input_runtime_test.cc"
    "${PROJECT_SOURCE_DIR}/src/plib/gnw/input_runtime.cc"
)

add_executable(loader_contract_test
    "loader_contract_test.cc"
    "${PROJECT_SOURCE_DIR}/src/game/config.cc"
    "${PROJECT_SOURCE_DIR}/src/game/gconfig.cc"
    "${PROJECT_SOURCE_DIR}/src/plib/assoc/assoc.cc"
    "${PROJECT_SOURCE_DIR}/src/plib/db/db.cc"
    "${PROJECT_SOURCE_DIR}/src/plib/db/lzss.cc"
    "${PROJECT_SOURCE_DIR}/src/platform_compat.cc"
)

add_executable(db_warning_regression_test
    "db_warning_regression_test.cc"
    "${PROJECT_SOURCE_DIR}/src/plib/assoc/assoc.cc"
    "${PROJECT_SOURCE_DIR}/src/plib/db/db.cc"
    "${PROJECT_SOURCE_DIR}/src/plib/db/lzss.cc"
    "${PROJECT_SOURCE_DIR}/src/platform_compat.cc"
)

target_include_directories(lifecycle_test PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

target_include_directories(input_runtime_test PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

target_include_directories(loader_contract_test PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

target_include_directories(db_warning_regression_test PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

target_link_libraries(lifecycle_test PRIVATE
    Catch2::Catch2WithMain
)

target_link_libraries(input_runtime_test PRIVATE
    Catch2::Catch2WithMain
)

target_link_libraries(loader_contract_test PRIVATE
    Catch2::Catch2WithMain
    SDL3::SDL3
    fpattern::fpattern
)

target_link_libraries(db_warning_regression_test PRIVATE
    Catch2::Catch2WithMain
    SDL3::SDL3
    fpattern::fpattern
)

include(Catch)
catch_discover_tests(pointer_registry_test)
catch_discover_tests(lifecycle_test
    TEST_PREFIX "lifecycle_test::"
)
catch_discover_tests(input_runtime_test
    TEST_PREFIX "input_runtime_test::"
)
catch_discover_tests(loader_contract_test
    TEST_PREFIX "loader_contract_test::"
)
catch_discover_tests(db_warning_regression_test
    TEST_PREFIX "db_warning_regression_test::"
)
