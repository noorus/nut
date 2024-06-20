#pragma once

#include "nut_types.hpp"

namespace nut {

  namespace platform {

    inline span<uint8_t> readRCDataResource( HMODULE mod, LPCWSTR name )
    {
      auto resource = FindResourceW( mod, name, RT_RCDATA );
      if ( !resource )
        return {};

      auto size = SizeofResource( mod, resource );
      if ( !size )
        return {};

      auto data = LoadResource( mod, resource );
      if ( !data )
        return {};

      auto buffer = static_cast<uint8_t*>( LockResource( data ) );
      if ( !buffer )
        return {};

      return { buffer, size };
    }

  }

}