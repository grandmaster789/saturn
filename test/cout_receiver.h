#ifndef SATURN_TEST_COUT_RECEIVER_H
#define SATURN_TEST_COUT_RECEIVER_H

#include <iostream>
#include <exception>

namespace saturn_test {
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

#endif