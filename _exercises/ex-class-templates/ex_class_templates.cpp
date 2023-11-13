#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

using Catch::Matchers::Equals;

// TODO: implement Stack class template

// static_assert(std::is_same_v<Stack<int>::container_type, std::deque<int>>);
// static_assert(std::is_same_v<Stack<int, std::vector<int>>::container_type, std::vector<int>>);

// TEST_CASE("After construction", "[stack]")
// {
//     Stack<int> s;

//     SECTION("is empty")
//     {
//         REQUIRE(s.empty());
//     }

//     SECTION("size is zero")
//     {
//         REQUIRE(s.size() == 0);
//     }
// }

// TEST_CASE("Pushing an item", "[stack,push]")
// {
//     Stack<int> s;

//     SECTION("is no longer empty")
//     {
//         s.push(1);

//         REQUIRE(!s.empty());
//     }

//     SECTION("size is increased")
//     {
//         auto size_before = s.size();

//         s.push(1);

//         REQUIRE(s.size() - size_before == 1);
//     }

//     SECTION("recently pushed item is on a top")
//     {
//         s.push(4);

//         REQUIRE(s.top() == 4);
//     }
// }

// template <typename TStack>
// std::vector<typename TStack::value_type> pop_all(TStack& s)
// {
//     std::vector<typename TStack::value_type> values(s.size());

//     for (auto& item : values)
//         s.pop(item);

//     return values;
// }

// TEST_CASE("Popping an item", "[stack,pop]")
// {
//     Stack<int> s;

//     s.push(1);
//     s.push(4);

//     int item;

//     SECTION(" an item from a top to an argument passed by ref")
//     {
//         s.pop(item);

//         REQUIRE(item == 4);
//     }

//     SECTION("size is decreased")
//     {
//         size_t size_before = s.size();

//         s.pop(item);

//         REQUIRE(size_before - s.size() == 1);
//     }

//     SECTION("LIFO order")
//     {
//         int a, b;

//         s.pop(a);
//         s.pop(b);

//         REQUIRE(a == 4);
//         REQUIRE(b == 1);
//     }
// }

// TEST_CASE("Copy semantics", "[stack,copy]")
// {
//     Stack<std::string> s;
//     s.push("txt1");
//     s.push("txt2");
//     s.push("txt3");

//     SECTION("copy constructor", "[stack,copy]")
//     {
//         Stack<std::string> copy_s = s;

//         REQUIRE_THAT(pop_all(copy_s), Equals(std::vector<std::string>{"txt3", "txt2", "txt1"}));
//     }

//     SECTION("copy assignment", "[stack,copy]")
//     {
//         SECTION("when the same capacity")
//         {
//             Stack<std::string> target;
//             target.push("x");
//             REQUIRE(target.size() == 1);

//             target = s;
//             REQUIRE(target.size() == 3);

//             REQUIRE_THAT(pop_all(target), Equals(std::vector<std::string>{"txt3", "txt2", "txt1"}));
//         }
//     }
// }

// TEST_CASE("Copy & conversion", "[stack,copy,conversion]")
// {
//     Stack<int> s_ints;
//     s_ints.push(1);
//     s_ints.push(2);
//     s_ints.push(3);

//     Stack<double> s_doubles = s_ints;

//     REQUIRE_THAT(pop_all(s_doubles), Equals(std::vector<double>{3.0, 2.0, 1.0}));
// }

// TEST_CASE("Copy & different containers", "[stack,copy,conversion]")
// {
//     Stack<int, std::deque<int>> s_ints;

//     s_ints.push(1);
//     s_ints.push(2);
//     s_ints.push(3);

//     Stack<int, std::vector<int>> s_ints_with_vec = s_ints;

//     REQUIRE(pop_all(s_ints_with_vec) == std::vector{3, 2, 1});
// }

// TEST_CASE("Copy & different containers with convertible types", "[stack,copy,conversion]")
// {
//     Stack<int, std::deque<int>> s_ints;
//     s_ints.push(1);
//     s_ints.push(2);
//     s_ints.push(3);

//     Stack<double, std::vector<double>> s_doubles_with_vec = s_ints;

//     REQUIRE(pop_all(s_doubles_with_vec) == std::vector{3.0, 2.0, 1.0});
// }