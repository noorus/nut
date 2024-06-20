#pragma once

#include "nut_types.hpp"

namespace nut {

  namespace platform {

    //! UTF-8 to wide string conversion.
    inline wstring utf8ToWide( string_view in ) noexcept
    {
      int length = MultiByteToWideChar( CP_UTF8, 0, in.data(), -1, nullptr, 0 );
      if ( length == 0 )
        return {};
      vector<wchar_t> conversion( length );
      MultiByteToWideChar( CP_UTF8, 0, in.data(), -1, &conversion[0], length );
      return &conversion[0];
    }

    //! Wide string to UTF-8 conversion.
    inline string wideToUtf8( wstring_view in ) noexcept
    {
      int length = WideCharToMultiByte( CP_UTF8, 0, in.data(), -1, nullptr, 0, nullptr, FALSE );
      if ( length == 0 )
        return {};
      vector<char> conversion( length );
      WideCharToMultiByte( CP_UTF8, 0, in.data(), -1, &conversion[0], length, nullptr, FALSE );
      return &conversion[0];
    }

  }

}