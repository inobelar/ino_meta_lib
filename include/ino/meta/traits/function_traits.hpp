#pragma once

#include <ino/meta/traits/type_at.hpp>

namespace ino {
namespace meta {
namespace traits {

/*
    Based on: https://stackoverflow.com/q/34283919/

    NOTE: for void funcs, arity 0, not 1. And in such case argument_type_at<0> not compiles.

    Not works with overloaded funcs:
          int func(bool, char) { return 42; }
        float func(bool, char, int) { return 3.14f; }

    See also:
        https://www.boost.org/doc/libs/1_74_0/libs/type_traits/doc/html/boost_typetraits/reference/function_traits.html
*/

template <typename>
struct function_traits;

// Specialization for lambda's, std::function's and other functors
template <typename Function>
struct function_traits
        : public function_traits< decltype(&Function::operator()) >
{};

// Specialization for member functions
template <
        typename    ClassType,
        typename    ReturnType,
        typename... Arguments
        >
struct function_traits<
        ReturnType(ClassType::*)(Arguments...) const
>
{
    using class_type  = ClassType;

    using result_type = ReturnType;

    template <std::size_t Index>
    using argument_type_at = typename traits::type_at<Index, Arguments...>::type;

    // Function arity = arguments count
    static constexpr std::size_t arity = sizeof...(Arguments);
};

// Specialization for common function
template <
        typename    ReturnType,
        typename... Arguments
        >
struct function_traits<
        ReturnType(*)(Arguments...)
>
{
    using result_type = ReturnType;

    template <std::size_t Index>
    using argument_type_at = typename traits::type_at<Index, Arguments...>::type;

    // Function arity = arguments count
    static constexpr std::size_t arity = sizeof...(Arguments);
};

} // namespace traits
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

#include <functional> // for std::function<R(Args...)>

namespace tests {

using namespace ino::meta::traits;

namespace common_functions_tests {

    namespace procedures_tests {

        void test_proc_implicit();
        void test_proc_explicit(void);

        // Arity check
        static_assert( function_traits<decltype(&test_proc_implicit)>::arity == 0, "Test failed");
        static_assert( function_traits<decltype(&test_proc_explicit)>::arity == 0, "Test failed");

        // Result type check
        static_assert(std::is_same< function_traits<decltype(&test_proc_implicit)>::result_type, void>::value == true, "Test failed");
        static_assert(std::is_same< function_traits<decltype(&test_proc_explicit)>::result_type, void>::value == true, "Test failed");

    } // namespace procedures_tests

    namespace functions_tests {

        void  test_func_1(bool);
        int   test_func_2(bool, char);
        float test_func_3(bool, char, int);

        // Arity check
        static_assert( function_traits<decltype(&test_func_1)>::arity == 1, "Test failed");
        static_assert( function_traits<decltype(&test_func_2)>::arity == 2, "Test failed");
        static_assert( function_traits<decltype(&test_func_3)>::arity == 3, "Test failed");

        // Result type check
        static_assert(std::is_same< function_traits<decltype(&test_func_1)>::result_type,  void>::value == true, "Test failed");
        static_assert(std::is_same< function_traits<decltype(&test_func_2)>::result_type,   int>::value == true, "Test failed");
        static_assert(std::is_same< function_traits<decltype(&test_func_3)>::result_type, float>::value == true, "Test failed");

        // Argument types check
        static_assert(std::is_same< function_traits<decltype(&test_func_1)>::argument_type_at<0>, bool>::value == true, "Test failed");

        static_assert(std::is_same< function_traits<decltype(&test_func_2)>::argument_type_at<0>, bool>::value == true, "Test failed");
        static_assert(std::is_same< function_traits<decltype(&test_func_2)>::argument_type_at<1>, char>::value == true, "Test failed");

        static_assert(std::is_same< function_traits<decltype(&test_func_3)>::argument_type_at<0>, bool>::value == true, "Test failed");
        static_assert(std::is_same< function_traits<decltype(&test_func_3)>::argument_type_at<1>, char>::value == true, "Test failed");
        static_assert(std::is_same< function_traits<decltype(&test_func_3)>::argument_type_at<2>,  int>::value == true, "Test failed");

    } // namespace functions_tests

} // namespace common_functions_tests

namespace std_function_tests {

    namespace procedures_tests {

        using func_t = std::function<int(void)>;

        // Arity check
        static_assert( function_traits< func_t >::arity == 0, "Test failed");

        // Result type check
        static_assert(std::is_same< function_traits< func_t >::result_type, int>::value == true, "Test failed");

    } // namespace procedures_tests

    namespace functions_tests {

        using func_t = std::function<float (bool, char, int)>;

        // Arity check
        static_assert( function_traits< func_t >::arity == 3, "Test failed");

        // Result type check
        static_assert(std::is_same< function_traits< func_t >::result_type, float>::value == true, "Test failed");

        // Argument type check
        static_assert(std::is_same< function_traits< func_t >::argument_type_at<0>, bool>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< func_t >::argument_type_at<1>, char>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< func_t >::argument_type_at<2>,  int>::value == true, "Test failed");

    } // namespace functions_tests

} // namespace std_function_tests

// TODO: lambda tests (with capture)

// TODO: struct operator () tests

struct dummy {
    static int value() { return 42; }
    float add(int a, int b) const { return a + b; }
};

static_assert( function_traits< decltype(&dummy::add) >::arity == 2, "asd");
static_assert( function_traits< decltype(&dummy::value) >::arity == 0, "asd");

static_assert(std::is_same< function_traits< decltype(&dummy::add) >::argument_type_at<0>, int>::value == true, "asd");

// TODO: class method check

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
