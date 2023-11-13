#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

template <typename T>
void foo(T obj)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

template <typename T>
void foo(T* ptr)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

TEST_CASE("SFINAE")
{
    foo(42);
}