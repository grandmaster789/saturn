#include <catch2/catch_test_macros.hpp>
#include "just.h"
#include "then.h"
#include "sync_wait.h"

TEST_CASE("Sync_wait(value)", "[core]") {
    using namespace saturn;

    auto x      = just(5);
    auto result = sync_wait(x);

    REQUIRE(result.has_value());
    REQUIRE(*result == 5);
}

TEST_CASE("Sync_wait(nested)", "[core]") {
    using namespace saturn;

    int result = 0;

    sync_wait(
        then(
            just(),
            [&] { result = 123; }
        )
    );

    REQUIRE(result == 123);
}

TEST_CASE("Sync_wait(piped)", "[core]") {
    using namespace saturn;

    auto result = just(6)
        | then([] (int i) { return i * 3; })
        | sync_wait;

    REQUIRE(result.has_value());
    REQUIRE(*result == 18);;
}