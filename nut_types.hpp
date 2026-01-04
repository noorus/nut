#pragma once

#include <cstdint>
#include <exception>
#include <stdexcept>
#include <optional>
#include <ranges>
#include <span>
#include <vector>
#include <string>

#include <windows.h>
#include <strsafe.h>

#ifdef NUT_CUSTOM_INCLUDE
# include NUT_CUSTOM_INCLUDE
#endif

#define NUT_CONFIG_WANT_THREAD_NAMING

namespace nut {

  template <typename Range, typename T>
  concept ContiguousRangeOf = ( std::ranges::contiguous_range<Range> && std::same_as<std::ranges::range_value_t<Range>, T> );

  using RuntimeError = std::runtime_error;

  using std::optional;
  using std::span;
  using std::string_view;
  using std::wstring_view;

#ifdef NUT_STRING_TYPE
  using string = NUT_STRING_TYPE;
#else
  using std::string;
#endif

#ifdef NUT_VECTOR_TYPE
  template <typename T>
  using vector = NUT_VECTOR_TYPE<T>;
#else
  using std::vector;
#endif

#ifdef NUT_WSTRING_TYPE
  using wstring = NUT_WSTRING_TYPE;
#else
  using std::wstring;
#endif

#define NUT_RUNTIME_EXCEPT( rsn ) throw nut::RuntimeError( rsn );

}