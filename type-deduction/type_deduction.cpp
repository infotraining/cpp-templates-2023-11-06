#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

void foo(int)
{ }

template <typename T>
void deduce1(T arg)
{
    puts(__PRETTY_FUNCTION__);
}

namespace Cpp20
{
    void deduce1(auto arg)
    {
        puts(__PRETTY_FUNCTION__);
    }
}

template <typename T>
void deduce2(T& arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce3(T&& arg)
{
    puts(__PRETTY_FUNCTION__);
}

TEST_CASE("type deduction rules")
{
    int x = 10;
    int& ref_x = x;
    const int& cref_x = x;
    int tab[10];
    const int cx = 10;

    SECTION("Case 1")
    {
        deduce1(x);      // int
        deduce1(cx);     // int - const is removed
        deduce1(ref_x);  // int - ref is removed
        deduce1(cref_x); // int - ref & const are removed
        deduce1(tab);    // int* - array decays to pointer
        deduce1(foo);    // void (*)(int)
        

        auto a1 = x;       // int
        auto a2 = ref_x;   // int
        auto a3 = cref_x;  // int
        auto a4 = tab;     // int*
        auto a5 = foo;     // void(*)(int)
        auto a6 = cx;      // int
    }

    SECTION("Case 2")
    {
        deduce2(x);      // deduce2<int>(int&) 
        deduce2(ref_x);  // deduce2<int>(int&)
        deduce2(cref_x); // deduce2<const int>(const int&)
        deduce2(tab);    // deduce2<int[10]>(int(&)[10]) - tab size is preserved
        deduce2(foo);    // deduce2<void(int)>(void(&)(int))
        deduce2(cx);     // deduce2<const int>(const int&)

        auto& a1 = x;      // int&
        auto& a2 = ref_x;  // int&
        auto& a3 = cref_x; // const int&
        auto& a4 = tab;    // int(&a4)[10]
        auto& a5 = foo;    // void (&)(int)
    }

    SECTION("Case 3")
    {
        deduce3(x);                // deduce3<int&>(int& && -> int&)             
        deduce3(ref_x);            // deduce3<int&>(int& && -> int&) 
        deduce3(cref_x);           // deduce3<const int&>(const int& && -> const int&)
        deduce3(tab);              // deduce3<int(&)[10]>(int(&)[10] && -> int(&)[10]
        deduce3(foo);              // deduce3<void(&)(int)>(void(&)(int) && -> void(&)(int))
        deduce3(cx);               // deduce3<const int&>(const int& && -> const int&)
        deduce3(string("text"));   // deduce3<std::string>(std::string&&)

        auto&& a1 = x;      // int&
        auto&& a2 = ref_x;  // int&
        auto&& a3 = cref_x; // const int&
        auto&& a4 = tab;    // int(&a4)[10]
        auto&& a5 = foo;    // void (&)(int)
        auto&& a6 = cx;
        auto&& a7 = string("text");
    }
}

struct Data
{

    std::vector<std::string> vec;

    //void add(const std::string& s)
    //{
    //    vec.push_back(s); //cc
    //}

    //void add(std::string&& s)
    //{
    //    vec.push_back(std::move(s));  // mv
    //}

    template <typename TArg>
    void add(TArg&& arg)
    {
        vec.push_back(std::forward<TArg>(arg));
    }

    /*void add(auto&& arg)
    {
        vec.push_back(std::forward<decltype(arg)>(arg));
    }*/
};

TEST_CASE("perfect forwarding")
{
    Data ds;

    std::string str = "abc";
    ds.add(str); // ds.add<std::string&>(std::string& && -> std::string& str) -> std::forward<std::string&>(str) -> str copy into vector
    ds.add(std::string("def")); // ds.add<std::string>(std::string&&) -> std::forward<std::string>(str) -> std::move(str) into vector
}


TEST_CASE("auto vs. decltype(auto)")
{
    const int cx = 10;
    const int& ref_cx = cx;

    auto ax1 = cx;     // int
    auto ax2 = ref_cx; // int
    decltype(auto) ax3 = cx; // const int
    decltype(auto) ax4 = ref_cx; // const int&
}


std::string bar()
{
    return "abc"s;
}

std::string& foobar()
{
    static std::string str = "def";

    return str;
}

template <typename F>
decltype(auto) call(F f)
{
    std::cout << "calling f!!!\n";
    return f();
}

TEST_CASE("call wrapper")
{
    std::string result = call(bar); // std::string

    std::string& ref_result = call(foobar); // std::string&
}