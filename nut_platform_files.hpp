#pragma once

#include "nut_types.hpp"

namespace nut {

  namespace platform {

    //! \class FileWriter
    //! Simple native file writer.
    class FileWriter {
    protected:
      FILE* file_ = nullptr;
    public:
      FileWriter( const char* filename, bool append = false )
      {
        file_ = fopen( filename, append ? "a" : "w" );

        if ( !file_  )
          NUT_RUNTIME_EXCEPT( "Failed to open file for writing" );
      }
      ~FileWriter()
      {
        if ( file_ )
          fclose( file_ );
      }
      void write( const void* buffer, uint32_t length )
      {
        if ( fwrite( buffer, 1, length, file_ ) < static_cast<size_t>( length ) )
          NUT_RUNTIME_EXCEPT( "Failed to write file" );
      }
      template <typename T>
      void write( T& value )
      {
        write( &value, sizeof( T ) );
      }
    };

  }

}
