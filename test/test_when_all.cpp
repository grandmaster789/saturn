#include <catch2/catch_test_macros.hpp>
#include "cout_receiver.h"
#include "expect_receiver.h"
#include "just.h"
#include "then.h"
#include "unsynchronized_wait.h"
#include "when_all.h"

TEST_CASE("when_all(value)", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    auto x    = when_all(just(4));
    auto recv = ExpectValueReceiver(4);

    auto op = x.connect(recv);
    op.start();
}

TEST_CASE("when_all(value, value)", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    auto x    = when_all(just(2), just(3));
    auto recv = ExpectValueReceiver(2, 3);

    auto op = x.connect(recv);
    op.start();
}

TEST_CASE("when_all(void)", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    int xx = 0;

    when_all(
        just(11) | then([&](int i) { xx = i; })
    ) | unsynchronized_wait;

    REQUIRE(xx == 11);
}

TEST_CASE("when_all(void, void)", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    int xx = 0;
    int yy = 0;

    auto a = just(22) | then([&](int i) { xx = i; });
    auto b = just(33) | then([&](int i) { yy = i; });

    when_all(a, b) | unsynchronized_wait;

    REQUIRE(xx == 22);
    REQUIRE(yy == 33);
}