#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std::literals;

namespace Step1
{
    template <typename T>
    T accumulate(const T* begin, const T* end)
    {
        T total = T{};

        while (begin != end)
        {
            total += *begin;
            ++begin;
        }

        return total;
    }
} // namespace Step1

TEST_CASE("accumulate - 1")
{
    using std::begin, std::end;

    uint8_t data[255];

    std::iota(begin(data), end(data), 1);

    auto result = Step1::accumulate(begin(data), end(data));

    // CHECK(result == 32640);  // should be 1 + 2 + 3 + ... + 255 = 32640 (uint8_t overflow)
}

namespace Step2
{
    template <typename T>
    struct AccumulationTraits;

    template <>
    struct AccumulationTraits<uint8_t>
    {
        typedef uintmax_t AccumulatorType;
    };

    template <>
    struct AccumulationTraits<float>
    {
        typedef double AccumulatorType;
    };

    template <typename T>
    typename AccumulationTraits<T>::AccumulatorType accumulate(const T* begin, const T* end)
    {
        using AccT = typename AccumulationTraits<T>::AccumulatorType;

        AccT total = T{};

        while (begin != end)
        {
            total += *begin;
            ++begin;
        }

        return total;
    }
} // namespace Step2

TEST_CASE("accumulate - 2")
{
    using std::begin, std::end;

    uint8_t data[255];

    std::iota(begin(data), end(data), 1);

    auto result = Step2::accumulate(begin(data), end(data));

    CHECK(result == 32640); // should be 1 + 2 + 3 + ... + 255 = 32640
}

namespace Step3
{
    template <typename T>
    struct AccumulationTraits;

    template <>
    struct AccumulationTraits<uint8_t>
    {
    public:
        typedef int AccumulatorType;
        static constexpr AccumulatorType const zero = 0;
    };

    template <>
    struct AccumulationTraits<int>
    {
    public:
        typedef long AccumulatorType;
        static constexpr AccumulatorType zero = 0;
    };

    template <typename T>
    typename AccumulationTraits<T>::AccumulatorType accumulate(T const* begin, T const* end)
    {
        using AccT = typename AccumulationTraits<T>::AccumulatorType;

        AccT total = AccumulationTraits<T>::zero; // usint zero defined in a traits class

        while (begin != end)
        {
            total += *begin;
            ++begin;
        }

        return total;
    }
} // namespace Step3

TEST_CASE("accumulate - 3")
{
    using std::begin, std::end;

    uint8_t data[255];

    std::iota(begin(data), end(data), 1);

    auto result = Step3::accumulate(begin(data), end(data));

    CHECK(result == 32640); // should be 1 + 2 + 3 + ... + 255 = 32640
}

namespace Step4
{
    template <typename T>
    struct AccumulationTraits;

    template <>
    struct AccumulationTraits<uint8_t>
    {
    public:
        typedef unsigned int AccumulatorType;
        static constexpr AccumulatorType const zero = 0;
    };

    template <>
    struct AccumulationTraits<int>
    {
    public:
        typedef long AccumulatorType;
        static constexpr AccumulatorType zero = 0;
    };

    template <typename T, typename Traits = AccumulationTraits<T>>
    typename Traits::AccumulatorType accumulate(T const* begin, T const* end)
    {
        using AccT = typename Traits::AccumulatorType;

        AccT total = Traits::zero;

        while (begin != end)
        {
            total += *begin;
            ++begin;
        }

        return total;
    }
} // namespace Step4

TEST_CASE("accumulate - 4")
{
    using std::begin, std::end;

    uint8_t data[255];

    std::iota(begin(data), end(data), 1);

    auto result = Step4::accumulate(begin(data), end(data));

    CHECK(result == 32640); // should be 1 + 2 + 3 + ... + 255 = 32640 (uint8_t overflow)
}

inline namespace Step5
{
    template <typename T>
    struct AccumulationTraits;

    template <>
    struct AccumulationTraits<uint8_t>
    {
        typedef unsigned int AccumulatorType;
        static constexpr AccumulatorType const zero = 0;
    };

    template <>
    struct AccumulationTraits<int>
    {
        typedef long AccumulatorType;
        static constexpr AccumulatorType zero = 0;
    };

    struct SumPolicy
    {
        template <typename T1, typename T2>
        static void accumulate(T1& total, const T2& value)
        {
            total += value;
        }
    };

    template <typename T, typename AccumulationPolicy = SumPolicy, typename Traits = AccumulationTraits<T>>
    typename Traits::AccumulatorType accumulate(T const* begin, T const* end)
    {
        using AccT = typename Traits::AccumulatorType;

        AccT total = Traits::zero;

        while (begin != end)
        {
            AccumulationPolicy::accumulate(total, *begin);
            ++begin;
        }

        return total;
    }
} // namespace Step5

struct MultiplyAccumulationTraits
{
    typedef unsigned int AccumulatorType;
    static constexpr AccumulatorType const zero = 1;
};

struct MultiplyPolicy
{
    template <typename T1, typename T2>
    static void accumulate(T1& total, const T2& value)
    {
        total *= value;
    }
};

TEST_CASE("accuumulate - 5")
{
    using namespace std;

    uint8_t data[255];

    iota(begin(data), end(data), 1);

    auto result = Step5::accumulate(begin(data), end(data));

    cout << "result: " << static_cast<int>(result) << endl;
}