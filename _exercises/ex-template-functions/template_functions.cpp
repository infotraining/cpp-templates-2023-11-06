#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>

using namespace std;

namespace TODO
{
    template <typename InputIter, typename Predicate>
    InputIter find_if(InputIter first, InputIter last, Predicate f)
    {
        for (InputIter it = first; it != last; ++it)
        {
            if (f(*it))
                return it;
        }

        return last;
    }

    namespace Alt
    {
        template <typename InputIter>
        InputIter find_if(InputIter first, InputIter last, std::function<bool(decltype(*first))> f)
        {
            for (InputIter it = first; it != last; ++it)
            {
                if (f(*it))
                    return it;
            }

            return last;
        }
    }


    template <typename InputIter, typename TResult>
    TResult accumulate(InputIter first, InputIter last, TResult val)
    {
        for (InputIter it = first; it != last; ++it)
        {
            val += *it;
        }

        return val;
    }

    template <typename InputIter>
    auto accumulate(InputIter first, InputIter last)
    {
        std::remove_cvref_t<decltype(*first)> val{};

        for (InputIter it = first; it != last; ++it)
        {
            val += *it;
        }

        return val;
    }
}

TEST_CASE("my find if")
{
     SECTION("happy path")
     {
         vector<int> vec = {1, 2, 3, 4, 665, 124, 4, 55, 234};

         auto is_lesser_evil = [](int x) {
             return x == 665;
         };

         auto pos = TODO::find_if(begin(vec), end(vec), is_lesser_evil);
         //auto pos = TODO::find_if(begin(vec), end(vec), [](int n) { return std::to_string(n); });

         REQUIRE(pos != end(vec));
         REQUIRE(*pos == 665);
     }

     SECTION("sad path")
     {
         vector<int> vec = {1, 2, 3, 4, 124, 4, 55, 234};

         auto is_lesser_evil = [](int x) {
             return x == 665;
         };

         auto pos = TODO::find_if(begin(vec), end(vec), is_lesser_evil);

         REQUIRE(pos == end(vec));
     }
}

TEST_CASE("my accumulate")
{
     const vector<int> data = {1, 2, 3, 4, 5};

     auto result = TODO::accumulate(begin(data), end(data));

     static_assert(std::is_same_v<decltype(result), int>);
     //static_assert(std::is_same_v<decltype(result), double>);

     REQUIRE(result == 15);
}