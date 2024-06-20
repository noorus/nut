#pragma once

#include "nut_types.hpp"
#include "nut_platform_strings.hpp"

namespace nut {

  namespace platform {

    namespace api {

      using fnSetThreadDescription = HRESULT( WINAPI* )( HANDLE hThread, PCWSTR lpThreadDescription );
      using fnGetThreadDescription = HRESULT( WINAPI* )( HANDLE hThread, PWSTR* ppszThreadDescription );

      struct WinapiCalls
      {
        fnSetThreadDescription pfnSetThreadDescription = nullptr;
        fnGetThreadDescription pfnGetThreadDescription = nullptr;
      };

      extern WinapiCalls g_calls;

      void initialize();
    }

    #ifdef NUT_CONFIG_WANT_THREAD_NAMING

    inline void setDebuggerThreadName_legacyImpl( HANDLE thread, const string& threadName )
    {
      auto threadID = GetThreadId( thread );
      #pragma pack( push, 8 )
      struct threadNamingStruct
      {
        DWORD type;
        LPCSTR name;
        DWORD threadID;
        DWORD flags;
      } nameSignalStruct = { 0 };
      #pragma pack( pop )
      nameSignalStruct.type = 0x1000;
      nameSignalStruct.name = threadName.c_str();
      nameSignalStruct.threadID = threadID;
      nameSignalStruct.flags = 0;
      __try
      {
        RaiseException( 0x406D1388, 0, sizeof( nameSignalStruct ) / sizeof( ULONG_PTR ),
          reinterpret_cast<const ULONG_PTR*>( &nameSignalStruct ) );
      }
      __except ( EXCEPTION_EXECUTE_HANDLER )
      {
      }
    }

    #endif

    //! Assign a thread name that will be visible in debuggers.
    inline void setDebuggerThreadName( HANDLE thread, const string& threadName )
    {
      #ifdef NUT_CONFIG_WANT_THREAD_NAMING
      // Prefer SetThreadDescription, but it is only available from Windows 10 version 1607 onwards.
      if ( api::g_calls.pfnSetThreadDescription )
        api::g_calls.pfnSetThreadDescription( thread, utf8ToWide( threadName ).c_str() );
      else
        setDebuggerThreadName_legacyImpl( thread, threadName );
      #endif
    }

    inline string getThreadDescriptor( HANDLE thread )
    {
      auto id = GetThreadId( thread );
      wchar_t* name = nullptr;
      if ( api::g_calls.pfnGetThreadDescription )
        api::g_calls.pfnGetThreadDescription( thread, &name );
      static thread_local wchar_t buf[128];
      if ( name )
      {
        StringCchPrintfW( buf, 128, L"0x%X (%s)", id, name );
        LocalFree( name );
      }
      else
        StringCchPrintfW( buf, 128, L"0x%X", id );
      return wideToUtf8( buf );
    }

    inline string getCurrentThreadDescriptor()
    {
      return getThreadDescriptor( GetCurrentThread() );
    }

  }

}