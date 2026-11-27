#pragma once

#include "nut_types.hpp"
#include "nut_platform_strings.hpp"

namespace nut {

  namespace platform {

    //! \class FileWriter
    //! Simple native file writer.
    class FileWriter {
    protected:
#ifdef NUT_PLATFORM_WINDOWS
      HANDLE file_ = INVALID_HANDLE_VALUE;
#elif NUT_PLATFORM_LINUX
      FILE* file_ = nullptr;
#endif
    public:
      FileWriter( const char* filename, bool append = false )
      {
#ifdef NUT_PLATFORM_WINDOWS
        auto widefilename = utf8ToWide( filename );
  
        file_ = CreateFileW( widefilename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, append ? OPEN_ALWAYS : CREATE_ALWAYS,
          FILE_ATTRIBUTE_NORMAL, nullptr );

        if ( file_ == INVALID_HANDLE_VALUE )
          NUT_RUNTIME_EXCEPT( "Failed to open file for writing" );

        if ( append )
          SetFilePointer( file_, 0, nullptr, FILE_END );
#elif NUT_PLATFORM_LINUX
        file_ = fopen( filename, append ? "a" : "w" );

        if ( !file_  )
          NUT_RUNTIME_EXCEPT( "Failed to open file for writing" );
#endif
      }
      void write( const void* buffer, uint32_t length )
      {
#ifdef NUT_PLATFORM_WINDOWS
        DWORD written = 0;
        auto ret = WriteFile( file_, buffer, length, &written, nullptr );
        if ( !ret || written != length )
          NUT_RUNTIME_EXCEPT( "Failed to write file" );
#elif NUT_PLATFORM_LINUX
        if ( fwrite( buffer, 1, length, file_ ) < static_cast<size_t>( length ) )
          NUT_RUNTIME_EXCEPT( "Failed to write file" );
#endif
      }
      ~FileWriter()
      {
#ifdef NUT_PLATFORM_WINDOWS
        if ( file_ != INVALID_HANDLE_VALUE )
          CloseHandle( file_ );
#elif NUT_PLATFORM_LINUX
        if ( file_ )
          fclose( file_ );
#endif
      }
    };

  }

}
