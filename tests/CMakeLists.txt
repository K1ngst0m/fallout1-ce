include(FetchContent)

FetchContent_Declare(Catch2
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG v3.13.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
include(FalloutTestHelpers)

fallout_add_test(pointer_registry_test
    SOURCES pointer_registry_test.cc "${PROJECT_SOURCE_DIR}/src/pointer_registry.cc"
)

fallout_add_test(lifecycle_test
    SOURCES lifecycle_test.cc "${PROJECT_SOURCE_DIR}/src/plib/gnw/lifecycle.cc"
)

fallout_add_test(input_runtime_test
    SOURCES input_runtime_test.cc "${PROJECT_SOURCE_DIR}/src/plib/gnw/input_runtime.cc"
)

fallout_add_test(loader_contract_test
    SOURCES loader_contract_test.cc
            "${PROJECT_SOURCE_DIR}/src/game/config.cc"
            "${PROJECT_SOURCE_DIR}/src/game/gconfig.cc"
            "${PROJECT_SOURCE_DIR}/src/plib/assoc/assoc.cc"
            "${PROJECT_SOURCE_DIR}/src/plib/db/db.cc"
            "${PROJECT_SOURCE_DIR}/src/plib/db/lzss.cc"
            "${PROJECT_SOURCE_DIR}/src/platform_compat.cc"
    LIBS SDL3::SDL3 fpattern::fpattern
)

fallout_add_test(db_warning_regression_test
    SOURCES db_warning_regression_test.cc
            "${PROJECT_SOURCE_DIR}/src/plib/assoc/assoc.cc"
            "${PROJECT_SOURCE_DIR}/src/plib/db/db.cc"
            "${PROJECT_SOURCE_DIR}/src/plib/db/lzss.cc"
            "${PROJECT_SOURCE_DIR}/src/platform_compat.cc"
    LIBS SDL3::SDL3 fpattern::fpattern
)
