#include "nut_platform_threads.hpp"

#ifdef NUT_CONFIG_WANT_THREAD_NAMING
#include "nut_platform_debugging.hpp"
#endif

namespace nut {

  namespace platform {

    Thread::Thread( const string& name, Callback callback, void* argument ):
      thread_( nullptr ), id_( 0 ), name_( name ), argument_( argument ), callback_( callback )
    {
    }

    DWORD Thread::threadProc( void* argument )
    {
      // TODO: catch exceptions
      auto self = (Thread*)argument;
      auto ret = self->callback_( self->run_, self->stop_, self->argument_ );
      return ( ret ? EXIT_SUCCESS : EXIT_FAILURE );
    }

    bool Thread::start()
    {
      thread_ = CreateThread( nullptr, 0, threadProc, this, CREATE_SUSPENDED, &id_ );
      if ( !thread_ )
        return false;

      #ifdef NUT_CONFIG_WANT_THREAD_NAMING
      setDebuggerThreadName( thread_, name_ );
      #endif

      if ( ResumeThread( thread_ ) == -1 )
        NUT_RUNTIME_EXCEPT( "Thread resume failed" );

      HANDLE events[2] = { run_.get(), thread_ };

      auto wait = WaitForMultipleObjects( 2, events, FALSE, INFINITE );
      if ( wait == WAIT_OBJECT_0 )
        return true;
      if ( wait == WAIT_OBJECT_0 + 1 )
        return false;

      NUT_RUNTIME_EXCEPT( "Thread wait failed" );

      return false;
    }

    void Thread::stop()
    {
      if ( thread_ )
      {
        stop_.set();
        WaitForSingleObject( thread_, INFINITE );
      }
      stop_.reset();
      run_.reset();
      thread_ = nullptr;
      id_ = 0;
    }

    bool Thread::waitFor( uint32_t milliseconds ) const
    {
      auto retval = WaitForSingleObject( thread_, milliseconds );
      return ( retval == WAIT_TIMEOUT );
    }

    Thread::~Thread()
    {
      stop();
    }

  }

}