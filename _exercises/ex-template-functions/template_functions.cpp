#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

namespace TODO
{
}

TEST_CASE("my find if")
{
    // SECTION("happy path")
    // {
    //     vector<int> vec = {1, 2, 3, 4, 665, 124, 4, 55, 234};

    //     auto is_lesser_evil = [](int x) {
    //         return x == 665;
    //     };

    //     auto pos = TODO::find_if(begin(vec), end(vec), is_lesser_evil);

    //     REQUIRE(pos != end(vec));
    //     REQUIRE(*pos == 665);
    // }

    // SECTION("sad path")
    // {
    //     vector<int> vec = {1, 2, 3, 4, 124, 4, 55, 234};

    //     auto is_lesser_evil = [](int x) {
    //         return x == 665;
    //     };

    //     auto pos = TODO::find_if(begin(vec), end(vec), is_lesser_evil);

    //     REQUIRE(pos == end(vec));
    // }
}

TEST_CASE("my accumulate")
{
    // const vector<int> data = {1, 2, 3, 4, 5};

    // auto result = TODO::accumulate(begin(data), end(data), 0.0);

    // static_assert(std::is_same_v<decltype(result), double>);

    // REQUIRE(result == 15);
}