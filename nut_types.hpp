#pragma once

#include <cstdint>
#include <exception>
#include <stdexcept>
#include <optional>
#include <ranges>
#include <span>
#include <vector>

#include <windows.h>

namespace nut {

  template <typename Range, typename T>
  concept ContiguousRangeOf = ( std::ranges::contiguous_range<Range> && std::same_as<std::ranges::range_value_t<Range>, T> );

  using RuntimeError = std::runtime_error;

  using std::optional;
  using std::span;
  using std::vector;

#define NUT_RUNTIME_EXCEPT( rsn ) throw nut::RuntimeError( rsn );

}