#include <catch2/catch_test_macros.hpp>
#include "expect_receiver.h"
#include "just.h"
#include "just_error.h"
#include "just_stopped.h"

TEST_CASE("Just(value)", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    auto x    = just(1);
    auto recv = ExpectValueReceiver(1);
    auto op   = x.connect(recv);
    op.start();
}

TEST_CASE("Just(error)", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    auto x = just_error(std::make_exception_ptr(std::runtime_error("test error")));
    auto recv = ExpectErrorReceiver();
    auto op = x.connect(recv);
    op.start();
}

TEST_CASE("Just(stopped)", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    auto x    = just_stopped();
    auto recv = ExpectStoppedReceiver();
    auto op   = x.connect(recv);
    op.start();
}