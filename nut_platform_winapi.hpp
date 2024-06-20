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

  }

}