#ifndef SATURN_TEST_EXPECT_RECEIVER_H
#define SATURN_TEST_EXPECT_RECEIVER_H

#include <catch2/catch_test_macros.hpp>
#include <tuple>

namespace saturn_test {
    template <typename... Ts>
    struct ExpectValueReceiver {
        std::tuple<Ts...> m_ExpectedValues;

        explicit ExpectValueReceiver(Ts&&... values):
            m_ExpectedValues(std::forward<Ts>(values)...)
        {
        }

        void set_value()                       { REQUIRE(false); }

        template <typename... Us>
        void set_value(Us&&... actual_values) {
            static_assert(sizeof...(Us) == sizeof...(Ts), "Mismatch in argument count");

            REQUIRE(std::make_tuple(std::forward<Us>(actual_values)...) == m_ExpectedValues);
        }

        void set_error(std::exception_ptr)     { REQUIRE(false); }
        void set_stopped()                     { REQUIRE(false); }
    };

    template <>
    struct ExpectValueReceiver<void> {
        void set_value()                       { }
        void set_error(std::exception_ptr)     { REQUIRE(false); }
        void set_stopped()                     { REQUIRE(false); }
    };

    struct ExpectErrorReceiver {
        template <typename... Ts>
        void set_value(Ts...)                  { REQUIRE(false); }
        void set_error(std::exception_ptr ptr) { REQUIRE(ptr); } // NOTE this will test for *any* exception, not a specific one
        void set_stopped()                     { REQUIRE(false); }
    };

    struct ExpectStoppedReceiver {
        template <typename...Ts>
        void set_value(Ts...)                  { REQUIRE(false); }
        void set_error(std::exception_ptr)     { REQUIRE(false); }
        void set_stopped()                     {}
    };
}

#endif
