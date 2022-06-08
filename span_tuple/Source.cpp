#include "span_tuple.h"
#include <array>
#include <iostream>

int main()
{
    std::array<int, 4> a1 = { 1, 2, 3, 4};
    std::array<float, 4> a2 = { 2.3f, 34.f, 12.3f, 32.f};
    int* ar = a1.data();
    int arr[4] = {};
    xk::span_tuple<int, 4, float> span(a1, a2);

    int* data = get<0>(span.data());
    int* data2 = span.data<0>();
    xk::span_tuple<int, 4, float>::reference r = span.front();

    float& f= span.front<1>();
    xk::span_tuple<int, 4, float>::reference r2 = span.back();
    xk::span_tuple<int, 4, float>::reference r3 = span[2];

    xk::span_tuple<int, std::dynamic_extent, float> span4(&ar[1], 2, a2.data());
    xk::span_tuple<int, std::dynamic_extent, float> span5(a1.begin(), a1.end(), a2.begin());



    float& f2= span.back<1>();


    std::span<int, 4> span3 = get<0>(span);
    std::span<int, 4> span2(a1);
    std::span<int, 4> span7(a1.begin(), a1.end());

    std::span<int> span6(span3);
    //xk::span_tuple<4, int> span3(ar, 4);

    static_assert(sizeof(std::tuple<int*, float*>) == sizeof(int*) + sizeof(float*));
    static_assert(sizeof(xk::span_tuple<int, 4, float>) == sizeof(int*) + sizeof(float*));
    static_assert(sizeof(xk::span_tuple<int, std::dynamic_extent, float>) == sizeof(int*) + sizeof(float*) + sizeof(size_t));

    {
        auto begin = span.begin();
        auto end = span.end();
        for(; begin != end; ++begin)
        {
            std::tuple<int&, float&> t = *begin;
            std::cout << "[" << get<0>(t) << ", " << get<1>(t) << "]\n";
        }
    }

    //for(std::tuple<int&, float&> r : span)
    //{

    //}


    //std::span<int, 4> a3;
}