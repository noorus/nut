#pragma once

#include "nut_types.hpp"

namespace nut {

  namespace platform {

    inline uint32_t getCurrentThreadID()
    {
      return ::GetCurrentThreadId();
    }

    inline HMODULE getCurrentDLLHandle()
    {
      HMODULE mod = nullptr;
      GetModuleHandleExW( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>( &getCurrentDLLHandle ), &mod );
      return mod;
    }

    inline string formatWinapiError( string_view func, DWORD code )
    {
      LPSTR message = nullptr;
      FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
        code, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), reinterpret_cast<LPSTR>( &message ), 0, nullptr );
      auto len = lstrlenA( message ) + func.size() + 64;
      auto fmtbuf = reinterpret_cast<char*>( LocalAlloc( LMEM_ZEROINIT, len ) );
      if ( !fmtbuf )
        return {};
      StringCchPrintfA( fmtbuf, len, "%s failed with 0x%8.8X: %s", func.data(), code, message );
      LocalFree( message );
      auto ret = string( fmtbuf );
      LocalFree( fmtbuf );
      return ret;
    }

  }

}