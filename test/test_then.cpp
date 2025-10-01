#include <catch2/catch_test_macros.hpp>
#include "cout_receiver.h"
#include "expect_receiver.h"
#include "just.h"
#include "then.h"

TEST_CASE("Then(value)", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    auto x = just(2);
    auto y = then(x, [](int val) { return val + 1; });
    auto recv = ExpectValueReceiver(3);

    auto op = y.connect(recv);
    op.start();
}

TEST_CASE("Then(void)", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    int xx = 0;

    auto x = just();
    auto y = then(x, [&] { xx = 234;} );
    auto op = y.connect(ExpectValueReceiver<void>());
    op.start();

    REQUIRE(xx == 234);
}

TEST_CASE("Then(mixed value/void)", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    int yy = 0;

    auto x = just(4);
    auto y = then(x, [&](int val) { yy = val; });
    auto z = then(y, []           {           });

    auto recv = ExpectValueReceiver<void>();
    auto op = z.connect(recv);
    op.start();

    REQUIRE(yy == 4);
}