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

namespace impl {

template <
    typename     ReturnType,
    typename ... Arguments
>
struct function_traits_base
{
    // Notice: to be similar to 'boost' here is used 'result_type', instead of 'return_type' :)
    using result_type = ReturnType;

    template <std::size_t Index>
    using argument_type_at = typename traits::type_at<Index, Arguments...>::type;

    // Function arity = arguments count
    static constexpr std::size_t arity = sizeof...(Arguments);
};

} // namespace impl

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename>
struct function_traits;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Specialization for pointer-to-function
template <
        typename     ReturnType,
        typename ... Arguments
>
struct function_traits< ReturnType (*) (Arguments...) >
    : impl::function_traits_base< ReturnType, Arguments ... >
{};

// Specialization for reference-to-function
template <
        typename     ReturnType,
        typename ... Arguments
>
struct function_traits< ReturnType (&) (Arguments...) >
    : impl::function_traits_base< ReturnType, Arguments ... >
{};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*
    Specialization for member function (methods of classes)

    ----------------------------------------------------------------------------

    Notice, that in macro below, used 'empty' params - to not add unnecessary
    qualifiers. It works at most compilers, accordingly to:
        https://stackoverflow.com/questions/7666344/are-empty-macro-arguments-legal-in-c11
    but, if it not - use 'placeholder' define, as described below:

    @code{.cpp}
        #define PLACEHOLDER
        // ...
        SPECIALIZATION_FOR_MEMBER_FUNCTION( const , PLACEHOLDER , PLACEHOLDER );
        // ...
        #undef PLACEHOLDER
    @endcode

    ----------------------------------------------------------------------------

    Macro-for-various-qualifiers inspired by:
        https://github.com/GeorgyFirsov/PatternMatching/blob/master/PatternMatching/ArgsOf.h
*/

#define SPECIALIZATION_FOR_MEMBER_FUNCTION( CONST_QUALIFIER, VOLATILE_QUALIFIER, REFERENCE_QUALIFIER )                        \
    template <                                                                                                                \
            typename     ReturnType,                                                                                          \
            typename     ClassType,                                                                                           \
            typename ... Arguments                                                                                            \
    >                                                                                                                         \
    struct function_traits< ReturnType (ClassType::*) (Arguments...) CONST_QUALIFIER VOLATILE_QUALIFIER REFERENCE_QUALIFIER > \
        : impl::function_traits_base< ReturnType, Arguments ... >                                                             \
    {                                                                                                                         \
        using class_type = ClassType;                                                                                         \
    };

// Non-const
SPECIALIZATION_FOR_MEMBER_FUNCTION(       ,         ,    );
SPECIALIZATION_FOR_MEMBER_FUNCTION(       ,         , &  );
SPECIALIZATION_FOR_MEMBER_FUNCTION(       ,         , && );
SPECIALIZATION_FOR_MEMBER_FUNCTION(       , volatile,    );
SPECIALIZATION_FOR_MEMBER_FUNCTION(       , volatile, &  );
SPECIALIZATION_FOR_MEMBER_FUNCTION(       , volatile, && );

// Const
SPECIALIZATION_FOR_MEMBER_FUNCTION( const ,         ,    );
SPECIALIZATION_FOR_MEMBER_FUNCTION( const ,         , &  );
SPECIALIZATION_FOR_MEMBER_FUNCTION( const ,         , && );
SPECIALIZATION_FOR_MEMBER_FUNCTION( const , volatile,    );
SPECIALIZATION_FOR_MEMBER_FUNCTION( const , volatile, &  );
SPECIALIZATION_FOR_MEMBER_FUNCTION( const , volatile, && );

#undef SPECIALIZATION_FOR_MEMBER_FUNCTION

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*
    Specialization for lambda's, std::function's and other functors/callables

    This produces the same data, as 'specialization for member function', except
    static methods - in that case 'specialization for pointer-to-function' being
    choosed.
*/
template <typename Functor>
struct function_traits
        : public function_traits< decltype(&Functor::operator()) >
{};

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

        // Class type check
        static_assert(std::is_same< function_traits< func_t >::class_type, func_t>::value == true, "Test failed");

    } // namespace procedures_tests

    namespace functions_tests {

        using func_t = std::function<float (bool, char, int)>;

        // Arity check
        static_assert( function_traits< func_t >::arity == 3, "Test failed");

        // Result type check
        static_assert(std::is_same< function_traits< func_t >::result_type, float>::value == true, "Test failed");

        // Class type check
        static_assert(std::is_same< function_traits< func_t >::class_type, func_t>::value == true, "Test failed");

        // Argument type check
        static_assert(std::is_same< function_traits< func_t >::argument_type_at<0>, bool>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< func_t >::argument_type_at<1>, char>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< func_t >::argument_type_at<2>,  int>::value == true, "Test failed");

    } // namespace functions_tests

} // namespace std_function_tests


namespace lambda_tests {

    /*
        To properly capturing variables, we need to make them non-static,
        because, otherwise we have the next warning:

            warning: capture of variable ‘bar’ with non-automatic storage duration

        That's why here is used function for making scope-local variables, and
        lambdas, that captures them.

        ------------------------------------------------------------------------

        The MAIN reason for testing lambdas - to make sure, that 'mutable'
        lambdas works!
    */
    void test_func_for_scope_restriction()
    {
        int  test_capture_int = 0;
        int* test_capture_ptr = &test_capture_int;

        static const auto test_lambda_0 = [] () {};
        static const auto test_lambda_1 = [] (int x) { return x; };

        // Lambdas with 'capture' non-static, since it is not good practice :)
        const auto test_lambda_2 = [test_capture_int] (int x, float y) { return test_capture_int + x + y; };
        const auto test_lambda_3 = [test_capture_ptr] (int x, float y, double z) mutable { *test_capture_ptr = x + y; return z; };

        // Arity check
        static_assert( function_traits< decltype(test_lambda_0) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(test_lambda_1) >::arity == 1, "Test failed");
        static_assert( function_traits< decltype(test_lambda_2) >::arity == 2, "Test failed");
        static_assert( function_traits< decltype(test_lambda_3) >::arity == 3, "Test failed");

        // Result type check
        static_assert(std::is_same< function_traits< decltype(test_lambda_0) >::result_type,   void>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(test_lambda_1) >::result_type,    int>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(test_lambda_2) >::result_type,  float>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(test_lambda_3) >::result_type, double>::value == true, "Test failed");

        // Class type check
        // TODO: WHAT?! WHY IT IS NOT WORK?!
        // static_assert(std::is_same< function_traits< decltype(test_lambda_0) >::class_type, decltype(test_lambda_0)>::value == true, "Test failed");
        // static_assert(std::is_same< function_traits< decltype(test_lambda_1) >::class_type, decltype(test_lambda_1)>::value == true, "Test failed");
        // static_assert(std::is_same< function_traits< decltype(test_lambda_2) >::class_type, decltype(test_lambda_2)>::value == true, "Test failed");
        // static_assert(std::is_same< function_traits< decltype(test_lambda_3) >::class_type, decltype(test_lambda_3)>::value == true, "Test failed");
    }

} // namespace lambda_tests

namespace struct_call_operator_tests {

    // struct:: operator () tests
    struct callable_0 { int   operator () (     )         { return 42; } };
    struct callable_1 { float operator () (float x) const { return  x; } };

    // Arity check
    static_assert( function_traits< callable_0 >::arity == 0, "Test failed");
    static_assert( function_traits< callable_1 >::arity == 1, "Test failed");

    // Result type check
    static_assert(std::is_same< function_traits< callable_0 >::result_type,   int>::value == true, "Test failed");
    static_assert(std::is_same< function_traits< callable_1 >::result_type, float>::value == true, "Test failed");

    // Class type check
    static_assert(std::is_same< function_traits< callable_0 >::class_type, callable_0>::value == true, "Test failed");
    static_assert(std::is_same< function_traits< callable_1 >::class_type, callable_1>::value == true, "Test failed");

    // Arguments type check
    static_assert(std::is_same< function_traits< callable_1 >::argument_type_at<0>, float>::value == true, "Test failed");

} // namespace struct_call_operator_tests


namespace class_tests {

    namespace basic_tests {

        class dummy
        {
            int value;

        public:

            float common_method(int a, int b) const { return a + b + value; }
            static int static_method(int x, int y, int z) { return x+y+z; }

            int common_getter() const { return value; }
            void common_setter(int v) { value = v; }
        };


        // Make sure, that passing 'static' methods works
        static_assert( function_traits< decltype(&dummy::static_method) >::arity == 3, "Test failed");

        // Make sure, that passing 'common' methods work
        static_assert( function_traits< decltype(&dummy::common_method) >::arity == 2, "Test failed");

        // Make sure (for paranoidal reasons), that common setters & getters work
        static_assert( function_traits< decltype(&dummy::common_getter) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&dummy::common_setter) >::arity == 1, "Test failed");


        // Class type check
        // static_assert(std::is_same< function_traits< decltype(&dummy::static_method) >::class_type, dummy>::value == true, "Test failed"); // TODO: Not works. no 'class_type', since this is not 'method', but function
        static_assert(std::is_same< function_traits< decltype(&dummy::common_method) >::class_type, dummy>::value == true, "Test failed");

        // Argument types check
        static_assert(std::is_same< function_traits< decltype(&dummy::static_method) >::argument_type_at<0>, int>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&dummy::common_method) >::argument_type_at<0>, int>::value == true, "Test failed");

    } // namespace basic_tests

    namespace call_operator_tests {

        class callable_0  { public: int operator () ()                   { return 42; } };
        class callable_1  { public: int operator () ()                &  { return 42; } };
        class callable_2  { public: int operator () ()                && { return 42; } };
        class callable_3  { public: int operator () ()       volatile    { return 42; } };
        class callable_4  { public: int operator () ()       volatile &  { return 42; } };
        class callable_5  { public: int operator () ()       volatile && { return 42; } };
        class callable_6  { public: int operator () () const             { return 42; } };
        class callable_7  { public: int operator () () const          &  { return 42; } };
        class callable_8  { public: int operator () () const          && { return 42; } };
        class callable_9  { public: int operator () () const volatile    { return 42; } };
        class callable_10 { public: int operator () () const volatile &  { return 42; } };
        class callable_11 { public: int operator () () const volatile && { return 42; } };

        // Arity checks
        static_assert( function_traits< callable_0  >::arity == 0, "Test failed");
        static_assert( function_traits< callable_1  >::arity == 0, "Test failed");
        static_assert( function_traits< callable_2  >::arity == 0, "Test failed");
        static_assert( function_traits< callable_3  >::arity == 0, "Test failed");
        static_assert( function_traits< callable_4  >::arity == 0, "Test failed");
        static_assert( function_traits< callable_5  >::arity == 0, "Test failed");
        static_assert( function_traits< callable_6  >::arity == 0, "Test failed");
        static_assert( function_traits< callable_7  >::arity == 0, "Test failed");
        static_assert( function_traits< callable_8  >::arity == 0, "Test failed");
        static_assert( function_traits< callable_9  >::arity == 0, "Test failed");
        static_assert( function_traits< callable_10 >::arity == 0, "Test failed");
        static_assert( function_traits< callable_11 >::arity == 0, "Test failed");

        // Class type checks
        static_assert(std::is_same< function_traits< callable_0  >::class_type, callable_0 >::value == true, "Test failed");
        static_assert(std::is_same< function_traits< callable_1  >::class_type, callable_1 >::value == true, "Test failed");
        static_assert(std::is_same< function_traits< callable_2  >::class_type, callable_2 >::value == true, "Test failed");
        static_assert(std::is_same< function_traits< callable_3  >::class_type, callable_3 >::value == true, "Test failed");
        static_assert(std::is_same< function_traits< callable_4  >::class_type, callable_4 >::value == true, "Test failed");
        static_assert(std::is_same< function_traits< callable_5  >::class_type, callable_5 >::value == true, "Test failed");
        static_assert(std::is_same< function_traits< callable_6  >::class_type, callable_6 >::value == true, "Test failed");
        static_assert(std::is_same< function_traits< callable_7  >::class_type, callable_7 >::value == true, "Test failed");
        static_assert(std::is_same< function_traits< callable_8  >::class_type, callable_8 >::value == true, "Test failed");
        static_assert(std::is_same< function_traits< callable_9  >::class_type, callable_9 >::value == true, "Test failed");
        static_assert(std::is_same< function_traits< callable_10 >::class_type, callable_10>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< callable_11 >::class_type, callable_11>::value == true, "Test failed");

        // Well ... I think, all works, no need to copy-paste 12 cases for 'return_type' & 'argument_type_at' checks :)

    } // namespace call_operator_tests

    namespace method_tests {

        class callable {
        public:
            int method_0 ()                   { return 42; }
            int method_1 ()                &  { return 42; }
            int method_2 ()                && { return 42; }
            int method_3 ()       volatile    { return 42; }
            int method_4 ()       volatile &  { return 42; }
            int method_5 ()       volatile && { return 42; }
            int method_6 () const             { return 42; }
            int method_7 () const          &  { return 42; }
            int method_8 () const          && { return 42; }
            int method_9 () const volatile    { return 42; }
            int method_10() const volatile &  { return 42; }
            int method_11() const volatile && { return 42; }
        };

        // Arity checks
        static_assert( function_traits< decltype(&callable::method_0 ) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&callable::method_1 ) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&callable::method_2 ) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&callable::method_3 ) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&callable::method_4 ) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&callable::method_5 ) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&callable::method_6 ) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&callable::method_7 ) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&callable::method_8 ) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&callable::method_9 ) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&callable::method_10) >::arity == 0, "Test failed");
        static_assert( function_traits< decltype(&callable::method_11) >::arity == 0, "Test failed");

        // Class type checks
        static_assert(std::is_same< function_traits< decltype(&callable::method_0 ) >::class_type, callable>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&callable::method_1 ) >::class_type, callable>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&callable::method_2 ) >::class_type, callable>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&callable::method_3 ) >::class_type, callable>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&callable::method_4 ) >::class_type, callable>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&callable::method_5 ) >::class_type, callable>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&callable::method_6 ) >::class_type, callable>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&callable::method_7 ) >::class_type, callable>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&callable::method_8 ) >::class_type, callable>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&callable::method_9 ) >::class_type, callable>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&callable::method_10) >::class_type, callable>::value == true, "Test failed");
        static_assert(std::is_same< function_traits< decltype(&callable::method_11) >::class_type, callable>::value == true, "Test failed");

        // Well ... I think, all works, no need to copy-paste 12 cases for 'return_type' & 'argument_type_at' checks :)

    } // namespace method_tests

} // namespace class_tests

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
