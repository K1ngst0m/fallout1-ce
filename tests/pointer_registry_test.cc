#include <catch2/catch_test_macros.hpp>

#include "pointer_registry.h"

namespace {

TEST_CASE("PointerRegistry stores nullptr as zero")
{
    fallout::PointerRegistry registry;

    REQUIRE(registry.store(nullptr) == 0);
    REQUIRE(registry.fetch(0) == nullptr);
}

TEST_CASE("PointerRegistry returns stored pointers")
{
    fallout::PointerRegistry registry;
    int value1 = 42;
    int value2 = 99;
    void* ptr1 = &value1;
    void* ptr2 = &value2;

    int ref1 = registry.store(ptr1);
    int ref2 = registry.store(ptr2);

    REQUIRE(ref1 != 0);
    REQUIRE(ref2 != 0);
    REQUIRE(ref1 != ref2);
    REQUIRE(registry.fetch(ref1) == ptr1);
    REQUIRE(registry.fetch(ref2) == ptr2);
}

TEST_CASE("PointerRegistry removes stored pointers when requested")
{
    fallout::PointerRegistry registry;
    int value = 42;
    void* ptr = &value;

    int ref = registry.store(ptr);

    REQUIRE(registry.fetch(ref, true) == ptr);
    REQUIRE(registry.fetch(ref) == nullptr);
}

TEST_CASE("PointerRegistry helper functions use the shared registry")
{
    int value = 42;
    void* ptr = &value;
    fallout::PointerRegistry* registry = fallout::PointerRegistry::shared();

    REQUIRE(fallout::PointerRegistry::shared() == registry);
    REQUIRE(fallout::ptrToInt(nullptr) == 0);
    REQUIRE(fallout::intToPtr(0) == nullptr);

    int ref = fallout::ptrToInt(ptr);

    REQUIRE(ref != 0);
    REQUIRE(fallout::intToPtr(ref) == ptr);
    REQUIRE(fallout::intToPtr(ref, true) == ptr);
    REQUIRE(fallout::intToPtr(ref) == nullptr);
}

} // namespace
