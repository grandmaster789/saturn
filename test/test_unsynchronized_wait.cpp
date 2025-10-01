#include <catch2/catch_test_macros.hpp>
#include "cout_receiver.h"
#include "just.h"
#include "then.h"
#include "unsynchronized_wait.h"

TEST_CASE("Unsynchronized_wait(value)", "[basic.extension]") {
    using namespace saturn;

    auto x      = just(55);
    auto result = unsynchronized_wait(x);

    REQUIRE(result.has_value());
    REQUIRE(*result == 55);
}

TEST_CASE("Unsynchronized_wait(piped)", "[basic.extension]") {
    using namespace saturn;

    auto result = just(66)
        | then([] (int i) { return i * 3; })
        | unsynchronized_wait;

    REQUIRE(result.has_value());
    REQUIRE(*result == 198);
}