.DEFAULT_GOAL := help

CONFIG_DEV_FILE ?= config.dev.ini
-include $(CONFIG_DEV_FILE)

BUILD_DIR ?= build/relwithdebinfo
DEBUG_BUILD_DIR ?= build/debug
ASAN_BUILD_DIR ?= build/asan
TEST_BUILD_DIR ?= build/test
WEB_BUILD_DIR ?= build/web

BUILD_TYPE ?= RelWithDebInfo
DEBUG_BUILD_TYPE ?= Debug
ASAN_BUILD_TYPE ?= Debug
TEST_BUILD_TYPE ?= Debug

GAME_DIR ?= ""
BIN_NAME ?= fallout-ce
BIN := $(BUILD_DIR)/$(BIN_NAME)
DEBUG_BIN := $(DEBUG_BUILD_DIR)/$(BIN_NAME)
ASAN_BIN := $(ASAN_BUILD_DIR)/$(BIN_NAME)

CMAKE ?= cmake
CMAKE_GENERATOR ?= Ninja
CTEST ?= ctest
JOBS ?= $(shell nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || printf '4')
CONFIGURE_FLAGS ?=
DEBUG_CONFIGURE_FLAGS ?=
ASAN_CONFIGURE_FLAGS ?=
RUN_ARGS ?=
RUN_DEBUGACTIVE ?= screen
FMT_SOURCES := $(shell find src tests -type f \( -name '*.cc' -o -name '*.h' \) 2>/dev/null)

.PHONY: help configure build rebuild debug asan configure-test test run run-debug run-asan web-configure web-build web-serve fmt fmt-check lint check clean

help: ## Show available targets and common overrides
	@printf "Developer-friendly targets for fallout1-ce\n\n"
	@printf "Config file:\n"
	@printf "  %s (loaded before built-in defaults; CLI overrides still win)\n\n" "$(CONFIG_DEV_FILE)"
	@printf "Common overrides:\n"
	@printf "  GAME_DIR=%s\n" "$(GAME_DIR)"
	@printf "  BUILD_DIR=%s DEBUG_BUILD_DIR=%s ASAN_BUILD_DIR=%s TEST_BUILD_DIR=%s WEB_BUILD_DIR=%s\n" "$(BUILD_DIR)" "$(DEBUG_BUILD_DIR)" "$(ASAN_BUILD_DIR)" "$(TEST_BUILD_DIR)" "$(WEB_BUILD_DIR)"
	@printf "  JOBS=%s CONFIGURE_FLAGS=... RUN_ARGS=... RUN_DEBUGACTIVE=%s\n\n" "$(JOBS)" "$(RUN_DEBUGACTIVE)"
	@printf "Targets:\n"
	@grep -h -E '^[a-zA-Z0-9_.-]+:.*## ' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*## "}; {printf "  %-12s %s\n", $$1, $$2}'

configure: ## Configure the default RelWithDebInfo build
	$(CMAKE) -G "$(CMAKE_GENERATOR)" -B "$(BUILD_DIR)" -DCMAKE_BUILD_TYPE="$(BUILD_TYPE)" $(CONFIGURE_FLAGS) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build: configure ## Build the default build directory
	$(CMAKE) --build "$(BUILD_DIR)" -j "$(JOBS)"

rebuild: clean build ## Remove build directories and rebuild release

debug: ## Configure and build the debug directory
	$(CMAKE) -G "$(CMAKE_GENERATOR)" -B "$(DEBUG_BUILD_DIR)" -DCMAKE_BUILD_TYPE="$(DEBUG_BUILD_TYPE)" $(CONFIGURE_FLAGS) $(DEBUG_CONFIGURE_FLAGS)
	$(CMAKE) --build "$(DEBUG_BUILD_DIR)" -j "$(JOBS)"

asan: ## Configure and build with ASAN and UBSAN
	$(CMAKE) -G "$(CMAKE_GENERATOR)" -B "$(ASAN_BUILD_DIR)" -DCMAKE_BUILD_TYPE="$(ASAN_BUILD_TYPE)" -DASAN=ON -DUBSAN=ON $(CONFIGURE_FLAGS) $(ASAN_CONFIGURE_FLAGS)
	$(CMAKE) --build "$(ASAN_BUILD_DIR)" -j "$(JOBS)"

configure-test: ## Configure the test build with Catch2 enabled
	$(CMAKE) -G "$(CMAKE_GENERATOR)" -B "$(TEST_BUILD_DIR)" -DCMAKE_BUILD_TYPE="$(TEST_BUILD_TYPE)" -DBUILD_TESTING=ON $(CONFIGURE_FLAGS)

test: configure-test ## Configure, build, and run Catch2 tests
	$(CMAKE) --build "$(TEST_BUILD_DIR)" -j "$(JOBS)"
	$(CTEST) --test-dir "$(TEST_BUILD_DIR)" --output-on-failure

run: build ## Build and run from GAME_DIR
	cd "$(GAME_DIR)" && DEBUGACTIVE="$(RUN_DEBUGACTIVE)" "$(abspath $(BIN))" $(RUN_ARGS)

run-debug: debug ## Build debug and run from GAME_DIR
	cd "$(GAME_DIR)" && DEBUGACTIVE="$(RUN_DEBUGACTIVE)" "$(abspath $(DEBUG_BIN))" $(RUN_ARGS)

run-asan: asan ## Build ASAN and run from GAME_DIR
	cd "$(GAME_DIR)" && DEBUGACTIVE="$(RUN_DEBUGACTIVE)" "$(abspath $(ASAN_BIN))" $(RUN_ARGS)

web-configure: ## Configure the Emscripten web build
	emcmake $(CMAKE) -G "$(CMAKE_GENERATOR)" -B "$(WEB_BUILD_DIR)" $(CONFIGURE_FLAGS)

web-build: web-configure ## Build the WebAssembly target
	$(CMAKE) --build "$(WEB_BUILD_DIR)" -j "$(JOBS)"

web-serve: ## Serve the web build locally on port 8080
	python3 -m http.server 8080 -d "$(WEB_BUILD_DIR)"

fmt: ## Format C and C++ sources under src/
	clang-format -i $(FMT_SOURCES)

fmt-check: ## Check formatting with clang-format
	clang-format --dry-run --Werror $(FMT_SOURCES)

lint: ## Run cppcheck over src/
	cppcheck --std=c++17 src/

check: fmt-check lint ## Run the lightweight verification suite

clean: ## Remove local build directories
	$(CMAKE) -E rm -rf "$(BUILD_DIR)" "$(DEBUG_BUILD_DIR)" "$(ASAN_BUILD_DIR)" "$(TEST_BUILD_DIR)" "$(WEB_BUILD_DIR)"
