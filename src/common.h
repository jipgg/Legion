#pragma once
#include <blaze/Blaze.h>
#include <cassert>
#define expects(precondition) assert(precondition and "expects")
#define ensures(postcondition) assert(postcondition and "ensures")
using V2 = blaze::StaticVector<float, 2>;
using M3x3 = blaze::StaticMatrix<float, 3, 2>;
template <class T>
concept Comparable_to_nullptr = std::is_convertible_v<decltype(std::declval<T>() != nullptr), bool>;
