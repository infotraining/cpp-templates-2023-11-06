#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

template <typename T>
T max_value(T a, T b)
{
    return a < b ? b : a;
}

TEST_CASE("function templates")
{
    CHECK(max_value(4, 5) == 5); // max_value<int>(4, 5)
    
    CHECK(max_value(4.0, 4.14) == 4.14); // max_value<double>(4.0, 4.14)

    std::string str1 = "abc";
    std::string str2 = "def";
    CHECK(max_value(str1, str2) == "def"s); // max_value<std::string>(str1, str2);

    CHECK(max_value(4.0f, 4.14f) == 4.14f); // max_value<float>(4.0f, 4.14f);

    CHECK(max_value(static_cast<double>(4), 4.14) == 4.14); // cast
    CHECK(max_value<double>(4, 4.14) == 4.14); // turn-off deduction
}