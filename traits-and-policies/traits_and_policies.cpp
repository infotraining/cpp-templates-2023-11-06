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

    CHECK(result == 32640); // should be 1 + 2 + 3 + ... + 255 = 32640 (uint8_t overflow)
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

    template <std::integral T>
    struct AccumulationTraits<T>
    {
    public:
        typedef std::intmax_t AccumulatorType;
        static constexpr AccumulatorType const zero = 0;
    };

    template <std::floating_point T>
    struct AccumulationTraits<T>
    {
    public:
        typedef double AccumulatorType;
        static constexpr AccumulatorType const zero = 0.0;
    };

    template <typename T>
    typename AccumulationTraits<T>::AccumulatorType accumulate(T const* begin, T const* end)
    {
        using AccT = typename AccumulationTraits<T>::AccumulatorType;

        AccT total = AccumulationTraits<T>::zero; // using zero defined in a traits class

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

TEST_CASE("accumulate - 5")
{
    using namespace std;

    uint8_t data[255];

    iota(begin(data), end(data), 1);

    auto result = Step5::accumulate<uint8_t, MultiplyPolicy, MultiplyAccumulationTraits>(begin(data), end(data));

    cout << "result: " << static_cast<int>(result) << endl;
}

////////////////////////////////////////////////////////////////////////////////
// dynamic polymorphism

namespace DynamicPoly
{

    class Service
    {
    public:
        virtual ~Service() = default;
        virtual std::string run() = 0;
    };

    class AService : public Service
    {
    public:
        std::string run() override
        {
            return "AService runs...";
        }
    };

    class AnotherService : public Service
    {
    public:
        std::string run() override
        {
            return "AnotherService runs...";
        }
    };

    class User
    {
        std::unique_ptr<Service> srv_;

    public:
        User(std::unique_ptr<Service> srv)
            : srv_(std::move(srv))
        { }

        void use()
        {
            std::cout << srv_->run() << "\n";
        }
    };
} // namespace DynamicPoly

namespace StaticPoly
{
    template<typename T>
    concept Service = requires(T srv) {
        { srv.run() } -> std::same_as<std::string>;
    };

    class AService
    {
    public:
        std::string run()
        {
            return "AService runs...";
        }
    };

    class AnotherService
    {
    public:
        std::string run()
        {
            return "AnotherService runs...";
        }
    };

    static_assert(Service<AnotherService>);

    template <Service TService>
    class User
    {
        TService srv_;

    public:
        User(TService srv)
            : srv_(std::move(srv))
        { }

        void use()
        {
            std::cout << srv_.run() << "\n";
        }
    };
} // namespace DynamicPoly

TEST_CASE("dynamic poly")
{
    DynamicPoly::User user1(std::make_unique<DynamicPoly::AnotherService>());
    user1.use();

    StaticPoly::User<StaticPoly::AService> user2{StaticPoly::AService()}; // Classic styleSer
    user2.use();

    StaticPoly::User user3{StaticPoly::AnotherService()}; // CTAD
    user3.use();
}

