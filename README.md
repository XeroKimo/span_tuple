# span_tuple
An extention to span. If span's intent is to be a non-owning range of one array-like container, span_tuple is meant to be a non-owning range of multiple array-like containers that have the same size. This can sometimes be seen in C functions, where it takes in one size param and multiple T* params where all their length is equal to the size param. This can be a drop in replacement to better express intent

# Tested On
- VS2022

# Examples
The original intent of this class is to be replace the following situations
```c++
void Foo(size_t arraySize, T* arrayOne, U* arrayTwo, V* arrayThree, int* someUnrelatedPointer);
```
Where all three arrays has the same size which was passed in as the first parameter. This can show up in old code and C code. It is then replaced with
```c++
void Foo(xk::span_tuple<std::dynamic_extent, T, U, V> spans, int* someUnrelatedPointer);
```
as it better expresses the intent in the code. We can decompose the the spans by doing the following
```c++
void Foo(xk::span_tuple<std::dynamic_extent, T, U, V> spans, int* someUnrelatedPointer)
{
  std::span<T> spanOne = xk::get<0>(spans);
  std::span<U> spanOne = xk::get<1>(spans);
  std::span<V> spanOne = xk::get<2>(spans);
}
```
and beyond that, will work exactly the same as usual. You can get the start of each span in the following ways
```c++
xk::span_tuple<std::dynamic_extent, T, U, V> spans;
get<0>(spans.data()); //returns T*;
spans.data<0>();
get<0>(spans).data();

get<1>(spans).data(); // returns U*
```

## Not Yet Implemented Examples
Access an element for each type of a given offset
```c++
xk::span_tuple<std::dynamic_extent, T, U, V> spans;
std::tuple<T&, U&, V&> elements = spans[3];
std::tuple<T&, U&, V&> elementsBegin = spans.begin();
std::tuple<T&, U&, V&> elementsEnd = spans.end();
```
Decompose all spans using structured bindings
```c++
auto [spanOne, spanTwo, spanThree] = spans;
```
For range loop
```c++
for(std::tuple<T*, U*, V*> elements : spans)
{
}
```

## TODO
- Implement range based and iterator based constructors
- Remove dependance on internal names from MSVCs std::span implementation
- Implement span_tuple's iterator
- Implement begin(), end(), front(), back() and operator\[\]() and other missing interfaces
- Create vcpkg package?
