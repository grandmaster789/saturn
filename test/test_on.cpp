#include <catch2/catch_test_macros.hpp>

#include "cout_receiver.h"

#include "just.h"
#include "then.h"
#include "sync_wait.h"
#include "unsynchronized_wait.h"
#include "inline_scheduler.h"
#include "on.h"

TEST_CASE("On inline scheduler", "[core]") {
    using namespace saturn;
    using namespace saturn_test;

    int zz = 0;

    InlineScheduler x2;

    sync_wait(
        on(x2, just(7))
        | then([&] (int i) { zz = i; })
    );

    REQUIRE(zz == 7);
 }

TEST_CASE("On inline scheduler pipe", "[core]") {
    using namespace saturn;

    int zz = 0;

    InlineScheduler x3;

    just(8)
     | on(x3)
     | then([&] (int i) { zz = i; })
     | unsynchronized_wait;

    REQUIRE(zz == 8);
 }