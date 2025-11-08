#pragma once

#include "nut_types.hpp"
#include "nut_platform_events.hpp"

namespace nut {

  namespace platform {

    //! \class Thread
    //! Native platform-dependent threading implementation for dedicated worker usage.
    class Thread {
    public:
      using Callback = bool(*)( Event& running, Event& wantStop, void* argument );
    protected:
      HANDLE thread_;
      DWORD id_;
      string name_;
      Event run_;
      Event stop_;
      void* argument_;
      Callback callback_;
      static DWORD WINAPI threadProc( void* argument );
    public:
      Thread( const string& name, Callback callback, void* argument );
      virtual bool start();
      virtual void stop();
      inline uint32_t id() const { return id_; }
      //! Returns true if thread is alive within given time
      virtual bool waitFor( uint32_t milliseconds = INFINITE ) const;
      //! Returns true if thread is alive
      inline bool check() const { return waitFor( 0 ); }
      ~Thread();
    };

  }

}