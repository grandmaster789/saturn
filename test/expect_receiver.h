#ifndef SATURN_TEST_EXPECT_RECEIVER_H
#define SATURN_TEST_EXPECT_RECEIVER_H

#include <catch2/catch_test_macros.hpp>

namespace saturn_test {
    template <typename T>
    struct ExpectValueReceiver {
        T m_Value;

        explicit ExpectValueReceiver(T value):
            m_Value(value)
        {
        }

        void set_value()                       { REQUIRE(false); }
        void set_value(T value)                { REQUIRE(value == m_Value); }
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
        template <typename T>
        void set_value(T)                      { REQUIRE(false); }
        void set_error(std::exception_ptr ptr) { REQUIRE(ptr); } // NOTE this will test for *any* exception, not a specific one
        void set_stopped()                     { REQUIRE(false); }
    };

    struct ExpectStoppedReceiver {
        template <typename T>
        void set_value(T)                      { REQUIRE(false); }
        void set_error(std::exception_ptr)     { REQUIRE(false); }
        void set_stopped()                     {}
    };
}

#endif
