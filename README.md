# span_tuple
An extention to span. If span's intent is to be a non-owning range of one array-like container, span_tuple is meant to be a non-owning range of multiple array-like containers that have the same size. This can sometimes be seen in C functions, where it takes in one size param and multiple T* params where all their length is equal to the size param. This can be a drop in replacement to better express intent

# Tested On
- VS2022

## TODO
- Implement range based and iterator based constructors
- Remove dependance on internal names from MSVCs std::span implementation
- Implement span_tuple's iterator
- Implement begin(), end(), front(), back() and operator\[\]() and other missing interfaces
- Create vcpkg package?
