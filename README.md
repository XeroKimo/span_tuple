# span_tuple
An extention to span. If span's intent is to be a non-owning range of one array-like container, span_tuple is meant to be a non-owning range of multiple array-like containers that have the same size. This can sometimes be seen in C functions, where it takes in one size param and multiple T* params where all their length is equal to the size param. This can be a drop in replacement to better express intent. 

The API works like a mix of [std::span](https://en.cppreference.com/w/cpp/container/span) and [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple), all constructors and the interface mirrors std::span, but since there are multiple types, we can turn them into individual spans using get like std::tuples.

xk::span_tuple currently assumes that either all spans are sized N or are sized 0. Currently thinking of an API to allow support for optional spans

See the [wiki](https://github.com/XeroKimo/span_tuple/wiki) for more info 
# Requires
- C++20
- std::span support
- concepts support
- feature test macros support

# Tested On
- VS2022

# Examples
The original intent of this class is to be replace the following situations
```c++
void Foo(size_t arraySize, T* arrayOne, U* arrayTwo, V* arrayThree, int* someUnrelatedPointer);
```
Where all three arrays has the same size which was passed in as the first parameter. This can show up in old code and C code. It is then replaced with
```c++
void Foo(xk::span_tuple<T, std::dynamic_extent, U, V> spans, int* someUnrelatedPointer);
```
as it better expresses the intent in the code. 

We can decompose the the spans by doing the following
```c++
void Foo(xk::span_tuple<T, std::dynamic_extent, U, V> spans, int* someUnrelatedPointer)
{
  std::span<T> spanOne = get<0>(spans);
  std::span<U> spanOne = get<1>(spans);
  std::span<V> spanOne = get<2>(spans);
}
```

You can get the start of each span in the following ways
```c++
xk::span_tuple<T, std::dynamic_extent, U, V> spans;
get<0>(spans.data()); //The following all returns T*;
spans.data<0>();
get<0>(spans).data();
get<T>(spans).data();

get<1>(spans).data(); // returns U*
```

Access an element for each type of a given offset
```c++
xk::span_tuple<T, std::dynamic_extent, U, V> spans;
std::tuple<T&, U&, V&> elements = spans[3];
std::tuple<T&, U&, V&> elementsBegin = spans.begin();
std::tuple<T&, U&, V&> elementsEnd = spans.end();
T& tBegin = spans.begin<0>();
U& uBegin = spans.begin<U>(); //Only works if there is no duplicate type just like a tuple
```
Decompose all spans using structured bindings
```c++
auto [spanOne, spanTwo, spanThree] = spans;
```
For range loop
```c++
for(std::tuple<T&, U&, V&> elements : spans)
{
}

for(auto [e1, e2, e3] : spans)
{
}
```

## TODO
~~- Implement range based and iterator based constructors~~

~~- Remove dependance on internal names from MSVCs std::span implementation~~

~~- Implement span_tuple's iterator~~

~~- Implement begin(), end(), front(), back() and operator\[\]() and other missing interfaces~~
- Create vcpkg package?
