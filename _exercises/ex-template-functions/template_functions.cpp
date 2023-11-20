#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <functional>
#include <iostream>
#include <list>
#include <numeric>
#include <string>
#include <vector>

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

    namespace Constrained
    {
        template <std::convertible_to<int32_t> T> // requires std::convertible_to<T, int32_t>
        void foo(T value)
        { }

        template <std::input_iterator InputIter, std::predicate<typename std::iterator_traits<InputIter>::value_type> F> // requires std::predicate<F, typename std::iterator_traits<InputIter>::value_type>
        InputIter find_if(InputIter first, InputIter last, F f)
        // requires std::predicate<F, decltype(*first)>
        {
            for (InputIter it = first; it != last; ++it)
            {
                if (f(*it))
                    return it;
            }

            return last;
        }
    } // namespace Constrained

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
    } // namespace Alt

    template <typename InputIter, typename TResult>
    TResult accumulate(InputIter first, InputIter last, TResult val)
    {
        for (InputIter it = first; it != last; ++it)
        {
            val += *it;
        }

        return val;
    }

    namespace Constrained
    {
        template <std::input_iterator InputIter, typename TResult>
            requires requires(TResult val, InputIter it) { val += *it; }
        TResult accumulate(InputIter first, InputIter last, TResult val)
        {
            for (InputIter it = first; it != last; ++it)
            {
                val += *it;
            }

            return val;
        }
    } // namespace Constrained

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

    namespace Constrained
    {
        template <std::input_iterator InputIter>
            requires 
                requires(std::iterator_traits<InputIter>::value_type val, InputIter it) { val += *it; } 
                    && std::default_initializable<typename std::iterator_traits<InputIter>::value_type>
        auto accumulate(InputIter first, InputIter last)
        {
            std::remove_cvref_t<decltype(*first)> val{};

            for (InputIter it = first; it != last; ++it)
            {
                val += *it;
            }

            return val;
        }
    } // namespace Constrained
} // namespace TODO

TEST_CASE("my find if")
{
    SECTION("happy path")
    {
        vector<int> vec = {1, 2, 3, 4, 665, 124, 4, 55, 234};

        auto is_lesser_evil = [](int x) {
            return x == 665;
        };

        auto pos = TODO::Constrained::find_if(begin(vec), end(vec), is_lesser_evil);
        // auto pos = TODO::find_if(begin(vec), end(vec), [](int n) { return std::to_string(n); });

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
    // static_assert(std::is_same_v<decltype(result), double>);

    REQUIRE(result == 15);
}