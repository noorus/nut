#include "nut_platform_debugging.hpp"

namespace nut {

  namespace platform {

    namespace api {

      WinapiCalls g_calls;

      void initialize()
      {
        auto kernel = LoadLibraryW( L"kernel32.dll" );
        if ( kernel )
        {
          g_calls.pfnSetThreadDescription = (fnSetThreadDescription)GetProcAddress( kernel, "SetThreadDescription" );
          g_calls.pfnGetThreadDescription = (fnGetThreadDescription)GetProcAddress( kernel, "GetThreadDescription" );
        }
      }

    }

  }

}