#include "just.h"
#include "then.h"
#include "sync_wait.h"
#include "unsynchronized_wait.h"
#include "inline_scheduler.h"
#include "on.h"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

namespace {
    // ~~ CoutReceiver may be useful in other contexts, but it's primarily handy for initial debugging

    template <typename... Ts>
    struct CoutVariadicPrinter {};

    template <typename T, typename... Ts>
    struct CoutVariadicPrinter<T, Ts...> {
        static void print(T&& value, Ts&&... values) {
            std::cout << std::forward<T>(value) << '\n';
            CoutVariadicPrinter<Ts...>::print(std::forward<Ts>(values)...);
        }
    };

    template <typename T>
    struct CoutVariadicPrinter<T> {
        static void print(T&& value) {
            std::cout << std::forward<T>(value) << '\n';
        }
    };

    template <>
    struct CoutVariadicPrinter<> {
        static void print() {
            std::cout << "Empty set\n";
        }
    };

    template <typename...Ts>
    void print_variadic(Ts&&... values) {
        CoutVariadicPrinter<Ts...> printer;
        printer.print(std::forward<Ts>(values)...);
    }

    struct CoutReceiver {
        void set_value() {
            std::cout << "set_value(void)\n";
        }

        template <typename...Ts>
        void set_value(Ts&&... values) {
            print_variadic(std::forward<Ts>(values)...);
        }

        void set_error(std::exception_ptr ptr) {
            try {
                if (ptr)
                    std::rethrow_exception(ptr);
            }
            catch (std::exception& ex) {
                std::cerr << "Caught exception " << ex.what() << '\n';
            }
            catch (...) {
                std::cerr << "Caught unknown exception" << '\n';
            }
        }

        void set_stopped() {
            std::cout << "set_stopped\n";
        }
    };
}

TEST_CASE("Just(value)", "[core]") {
    using namespace saturn;

    CoutReceiver recv;
    auto x  = just(1);
    auto op = x.connect(recv);
    op.start();
}

TEST_CASE("Then(value)", "[core]") {
    using namespace saturn;

    CoutReceiver recv;
    auto x = just(2);
    auto y = then(x, [](int val) { return val + 1; });
    auto op = y.connect(recv);
    op.start();
}

TEST_CASE("Then(void)", "[core]") {
    using namespace saturn;

    auto x = just();
    auto y = then(x, [] { std::cout << "then: void\n"; });
    auto op = y.connect(CoutReceiver{});
    op.start();
}

TEST_CASE("Then(mixed value/void)", "[core]") {
    using namespace saturn;

    CoutReceiver recv;
    auto x = just(4);
    auto y = then(x, [](int val) { std::cout << "then: " << val << '\n'; });
    auto z = then(y, []          { std::cout << "then: void\n";          });
    auto op = z.connect(recv);
    op.start();
}

TEST_CASE("Sync_wait(value)", "[core]") {
    using namespace saturn;

    auto x      = just(5);
    auto result = sync_wait(x);

    std::cout << "sync_wait: " << *result << '\n';
}

TEST_CASE("Sync_wait(nested)", "[core]") {
    using namespace saturn;

    sync_wait(
        then(
            just(),
            [] { std::cout << "sync_wait then void\n"; }
        )
    );
}

TEST_CASE("Sync_wait(piped)", "[core]") {
    using namespace saturn;

    auto result = just(6)
        | then([] (int i) { return i * 3; })
        | sync_wait;

    std::cout << "sync_wait piped: " << *result << '\n';
}

TEST_CASE("Unsynchronized_wait(value)", "[basic.extension]") {
    using namespace saturn;

    auto x      = just(55);
    auto result = unsynchronized_wait(x);

    std::cout << "unsynchronized_wait: " << *result << '\n';
}

TEST_CASE("Unsynchronized_wait(piped)", "[basic.extension]") {
    using namespace saturn;

    auto result = just(66)
        | then([] (int i) { return i * 3; })
        | unsynchronized_wait;

    std::cout << "unsynchronized_wait piped: " << *result << '\n';
}

TEST_CASE("On inline scheduler", "[core]") {
    using namespace saturn;

    InlineScheduler x2;

    sync_wait(
        on(x2, just(7))
        | then([] (int i) { std::cout << "x2 piped on: " << i << '\n'; })
    );
}

TEST_CASE("On inline scheduler pipe", "[core]") {
    using namespace saturn;

    InlineScheduler x3;

    just(8)
    | on(x3)
    | then([] (int i) { std::cout << "x3 piped on: " << i << '\n'; })
    | unsynchronized_wait;
}