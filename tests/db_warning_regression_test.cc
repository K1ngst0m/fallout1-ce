#include <catch2/catch_test_macros.hpp>

#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

#include "plib/db/db.h"

namespace {

static fallout::DB_DATABASE* const kInvalidDatabaseHandle = reinterpret_cast<fallout::DB_DATABASE*>(-1);

class ScopedTempDir {
public:
    explicit ScopedTempDir(const std::string& prefix)
    {
        static int counter = 0;
        const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = std::filesystem::temp_directory_path() / std::filesystem::path(prefix + "-" + std::to_string(now) + "-" + std::to_string(counter++));
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

std::uint32_t asBits(float value)
{
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}

TEST_CASE("db float read/write preserves exact bit patterns")
{
    ScopedTempDir tempDir("fallout-db-float-roundtrip");

    std::string patchPath = tempDir.path().string();
    fallout::DB_DATABASE* handle = fallout::db_init(nullptr, nullptr, patchPath.c_str(), 1);
    REQUIRE(handle != kInvalidDatabaseHandle);

    ScopedDbHandle cleanup(handle);
    REQUIRE(fallout::db_select(handle) == 0);

    fallout::DB_FILE* writer = fallout::db_fopen("float_roundtrip.bin", "wb");
    REQUIRE(writer != nullptr);

    const std::array<float, 5> expectedValues = {
        0.0f,
        1.25f,
        -42.5f,
        123456.25f,
        0.33333334f,
    };

    for (float expectedValue : expectedValues) {
        REQUIRE(fallout::db_fwriteFloat(writer, expectedValue) == 0);
    }

    REQUIRE(fallout::db_fclose(writer) == 0);

    fallout::DB_FILE* reader = fallout::db_fopen("float_roundtrip.bin", "rb");
    REQUIRE(reader != nullptr);

    for (float expectedValue : expectedValues) {
        float actualValue = 0.0f;
        REQUIRE(fallout::db_freadFloat(reader, &actualValue) == 0);
        CHECK(asBits(actualValue) == asBits(expectedValue));
    }

    REQUIRE(fallout::db_fclose(reader) == 0);
}

TEST_CASE("db_freadFloat reports failure when source stream is truncated")
{
    ScopedTempDir tempDir("fallout-db-float-short-read");

    std::ofstream(tempDir.path() / "truncated_float.bin", std::ios::binary)
        .write("\x12\x34\x56", 3);

    std::string patchPath = tempDir.path().string();
    fallout::DB_DATABASE* handle = fallout::db_init(nullptr, nullptr, patchPath.c_str(), 1);
    REQUIRE(handle != kInvalidDatabaseHandle);

    ScopedDbHandle cleanup(handle);
    REQUIRE(fallout::db_select(handle) == 0);

    fallout::DB_FILE* reader = fallout::db_fopen("truncated_float.bin", "rb");
    REQUIRE(reader != nullptr);

    float value = 0.0f;
    CHECK(fallout::db_freadFloat(reader, &value) == -1);

    REQUIRE(fallout::db_fclose(reader) == 0);
}

} // namespace
