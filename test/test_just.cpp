#include <catch2/catch_test_macros.hpp>
#include "expect_receiver.h"
#include "just.h"

TEST_CASE("Just(value)", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    auto x    = just(1);
    auto recv = ExpectValueReceiver(1);
    auto op   = x.connect(recv);
    op.start();
}