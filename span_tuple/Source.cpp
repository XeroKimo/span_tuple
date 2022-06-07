#include "span_tuple.h"
#include <array>

int main()
{
    std::array<int, 4> a1 = { 1, 2, 3, 4};
    std::array<float, 4> a2 = { 2.3f, 34.f, 12.3f, 32.f};
    int* ar = a1.data();
    int arr[4] = {};
    xk::span_tuple<4, int, float> span(a1, a2);

    int* data = get<0>(span.data());
    std::tuple<int*, float*> data2 = span.data<0, 1>();

    xk::span_tuple<4, int> span3 = get<0>(span);
    xk::span_tuple<4, int> span2(a1);
    //xk::span_tuple<4, int> span3(ar, 4);

    static_assert(sizeof(std::tuple<int*, float*>) == sizeof(int*) + sizeof(float*));
    static_assert(sizeof(xk::span_tuple<4, int, float>) == sizeof(int*) + sizeof(float*));
    static_assert(sizeof(xk::span_tuple<std::dynamic_extent, int, float>) == sizeof(int*) + sizeof(float*) + sizeof(size_t));


    //std::span<int, 4> a3;
}