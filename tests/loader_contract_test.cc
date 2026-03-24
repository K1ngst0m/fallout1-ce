#include <catch2/catch_test_macros.hpp>

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "game/config.h"
#include "game/gconfig.h"
#include "platform_compat.h"
#include "plib/db/db.h"
#include "plib/gnw/memory.h"

namespace fallout {

char* mem_strdup(const char* string)
{
    if (string == nullptr) {
        return nullptr;
    }

    size_t size = std::strlen(string) + 1;
    auto* copy = static_cast<char*>(std::malloc(size));
    if (copy != nullptr) {
        std::memcpy(copy, string, size);
    }

    return copy;
}

void* mem_malloc(size_t size)
{
    return std::malloc(size);
}

void* mem_realloc(void* ptr, size_t size)
{
    return std::realloc(ptr, size);
}

void mem_free(void* ptr)
{
    std::free(ptr);
}

void mem_check()
{
}

void mem_register_func(MallocFunc* /*mallocFunc*/, ReallocFunc* /*reallocFunc*/, FreeFunc* /*freeFunc*/)
{
}

} // namespace fallout

namespace {

static fallout::DB_DATABASE* const kInvalidDatabaseHandle = reinterpret_cast<fallout::DB_DATABASE*>(-1);

class ScopedTempDir {
public:
    explicit ScopedTempDir(const std::string& prefix)
    {
        static int counter = 0;
        path_ = std::filesystem::temp_directory_path() / std::filesystem::path(prefix + "-" + std::to_string(fallout::compat_timeGetTime()) + "-" + std::to_string(counter++));
        std::filesystem::create_directories(path_);
    }

    ~ScopedTempDir()
    {
        std::error_code error;
        std::filesystem::remove_all(path_, error);
    }

    const std::filesystem::path& path() const
    {
        return path_;
    }

private:
    std::filesystem::path path_;
};

class ScopedWorkingDirectory {
public:
    explicit ScopedWorkingDirectory(const std::filesystem::path& path)
        : previous_(std::filesystem::current_path())
    {
        std::filesystem::current_path(path);
    }

    ~ScopedWorkingDirectory()
    {
        std::filesystem::current_path(previous_);
    }

private:
    std::filesystem::path previous_;
};

class ScopedDbHandle {
public:
    explicit ScopedDbHandle(fallout::DB_DATABASE* handle)
        : handle_(handle)
    {
    }

    ~ScopedDbHandle()
    {
        if (handle_ != kInvalidDatabaseHandle) {
            fallout::db_close(handle_);
        }

        fallout::db_exit();
    }

private:
    fallout::DB_DATABASE* handle_;
};

class ScopedGconfig {
public:
    ScopedGconfig() = default;

    ~ScopedGconfig()
    {
        if (initialized_) {
            fallout::gconfig_exit(false);
        }
    }

    void markInitialized()
    {
        initialized_ = true;
    }

    void markExited()
    {
        initialized_ = false;
    }

private:
    bool initialized_ = false;
};

std::string readDbFile(fallout::DB_FILE* stream, int length)
{
    std::vector<unsigned char> bytes(length, 0);
    size_t readCount = fallout::db_fread(bytes.data(), 1, bytes.size(), stream);
    REQUIRE(readCount == bytes.size());

    return std::string(bytes.begin(), bytes.end());
}

TEST_CASE("Gconfig initializes loader defaults for game databases")
{
    ScopedTempDir tempDir("fallout-loader-gconfig");
    ScopedWorkingDirectory cwd(tempDir.path());
    ScopedGconfig configCleanup;

    char executablePath[] = "loader_contract_test";
    char* argv[] = { executablePath };

    REQUIRE(fallout::gconfig_init(false, 1, argv));
    configCleanup.markInitialized();

    char* masterDat = nullptr;
    char* critterDat = nullptr;
    char* masterPatches = nullptr;
    char* critterPatches = nullptr;

    REQUIRE(fallout::config_get_string(&fallout::game_config,
        GAME_CONFIG_SYSTEM_KEY,
        GAME_CONFIG_MASTER_DAT_KEY,
        &masterDat));
    REQUIRE(fallout::config_get_string(&fallout::game_config,
        GAME_CONFIG_SYSTEM_KEY,
        GAME_CONFIG_CRITTER_DAT_KEY,
        &critterDat));
    REQUIRE(fallout::config_get_string(&fallout::game_config,
        GAME_CONFIG_SYSTEM_KEY,
        GAME_CONFIG_MASTER_PATCHES_KEY,
        &masterPatches));
    REQUIRE(fallout::config_get_string(&fallout::game_config,
        GAME_CONFIG_SYSTEM_KEY,
        GAME_CONFIG_CRITTER_PATCHES_KEY,
        &critterPatches));

    CHECK(std::string(masterDat) == "master.dat");
    CHECK(std::string(critterDat) == "critter.dat");
    CHECK(std::string(masterPatches) == "data");
    CHECK(std::string(critterPatches) == "data");

    REQUIRE(fallout::gconfig_exit(false));
    configCleanup.markExited();
}

TEST_CASE("Database reads use patch-dir-first behavior when patch content exists")
{
    ScopedTempDir tempDir("fallout-loader-db");
    std::filesystem::path patchRoot = tempDir.path() / "data";
    std::filesystem::create_directories(patchRoot / "contracts");

    const std::string payload = "patch-overrides-archive";
    std::ofstream(patchRoot / "contracts" / "loader.txt", std::ios::binary) << payload;

    std::string patchPath = patchRoot.string();
    fallout::DB_DATABASE* handle = fallout::db_init(nullptr, nullptr, patchPath.c_str(), 1);
    REQUIRE(handle != kInvalidDatabaseHandle);

    ScopedDbHandle cleanup(handle);

    REQUIRE(fallout::db_select(handle) == 0);

    constexpr const char* logicalName = "contracts\\loader.txt";

    fallout::dir_entry entry = {};
    REQUIRE(fallout::db_dir_entry(logicalName, &entry) == 0);
    CHECK(entry.flags == 4);
    CHECK(entry.length == static_cast<int>(payload.size()));

    std::vector<unsigned char> buffer(payload.size(), 0);
    REQUIRE(fallout::db_read_to_buf(logicalName, buffer.data()) == 0);
    CHECK(std::string(buffer.begin(), buffer.end()) == payload);

    fallout::DB_FILE* stream = fallout::db_fopen(logicalName, "rb");
    REQUIRE(stream != nullptr);

    CHECK(readDbFile(stream, static_cast<int>(payload.size())) == payload);
    REQUIRE(fallout::db_fclose(stream) == 0);
}

} // namespace
