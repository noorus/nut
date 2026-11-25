#pragma once

#include "nut_types.hpp"

namespace nut {

  namespace platform {

#ifdef NUT_PLATFORM_WINDOWS

    using NativeEventType = HANDLE;

    //! \class Event
    //! Native event type abstraction for the current platform.
    class Event {
    private:
      NativeEventType handle_ = nullptr;
    public:
      Event( bool initialState = false )
      {
        handle_ = CreateEventW( nullptr, TRUE, initialState ? TRUE : FALSE, nullptr );
        if ( !handle_ )
          NUT_RUNTIME_EXCEPT( "Event creation failed" );
      }
      inline void set()
      {
        SetEvent( handle_ );
      }
      inline void reset()
      {
        ResetEvent( handle_ );
      }
      inline bool wait( uint32_t milliseconds = INFINITE ) const
      {
        return ( WaitForSingleObject( handle_, milliseconds ) == WAIT_OBJECT_0 );
      }
      inline bool check() const
      {
        return ( WaitForSingleObject( handle_, 0 ) == WAIT_OBJECT_0 );
      }
      inline NativeEventType get() const
      {
        return handle_;
      }
      ~Event()
      {
        if ( handle_ )
          CloseHandle( handle_ );
      }
    };

    //! Synchronously wait for one or all of native events to fire, or timeout.
    //! Returns the index of the fired event, or empty if the wait timed out.
    template <ContiguousRangeOf<NativeEventType> NativeEvents>
    inline optional<size_t> waitForEvents( const NativeEvents& events, uint32_t milliseconds, bool waitForAll = false )
    {
      auto ret = static_cast<size_t>( WaitForMultipleObjects(
        static_cast<DWORD>( events.size() ), events.data(), waitForAll, milliseconds > 0 ? milliseconds : INFINITE ) );
      if ( ret >= WAIT_OBJECT_0 && ret < ( WAIT_OBJECT_0 + events.size() ) )
        return ( ret - WAIT_OBJECT_0 );
      if ( ret == WAIT_TIMEOUT )
        return {};
      if ( ret >= WAIT_ABANDONED_0 && ret < ( WAIT_ABANDONED_0 + events.size() ) )
        NUT_RUNTIME_EXCEPT( "A wait object was abandoned and we don't deal with that" );
      NUT_RUNTIME_EXCEPT( "Event wait failed" );
    }

    inline optional<size_t> waitForEvents( const std::initializer_list<NativeEventType>& events, uint32_t milliseconds, bool waitForAll = false )
    {
      return waitForEvents( vector<NativeEventType>( events.begin(), events.end() ), milliseconds, waitForAll );
    }

#elif NUT_PLATFORM_LINUX

    using NativeEventType = int;

    //! \class Event
    //! Native event type abstraction for the current platform.
    class Event {
    public:
      constexpr static uint32_t c_infinity = 0xffffffff;
    private:
      NativeEventType handle_ = -1;
    public:
      Event( bool initialState = false )
      {
        handle_ = eventfd( initialState ? 1 : 0, EFD_NONBLOCK | EFD_CLOEXEC );
        if ( handle_ == -1 )
          NUT_RUNTIME_EXCEPT( "Event creation failed" );
      }
      inline void set()
      {
        uint64_t v = 1;
        if ( write( handle_, &v, sizeof( v ) ) != sizeof( v ) )
          NUT_RUNTIME_EXCEPT( "eventfd write failed" );
      }
      inline void reset()
      {
        uint64_t v = 0;
        if ( read( handle_, &v, sizeof( v ) ) < 0 && errno != EAGAIN )
          NUT_RUNTIME_EXCEPT( "eventfd read failed" );
      }
      inline bool wait( uint32_t milliseconds = c_infinity ) const
      {
        pollfd p;
        int ret = -1;
        errno = EINTR;
        while ( ret < 0 && errno == EINTR )
        {
          p.fd = handle_;
          p.events = POLLIN;
          p.revents = 0;
          auto to = ( milliseconds == c_infinity ? -1 : static_cast<int>( milliseconds ) );
          ret = poll( &p, 1, to );
        }
        if ( ret < 0 )
          return false;
        if ( ret == 0 || !( p.revents & POLLIN ) )
        {
          errno = 0;
          return false;
        }
        return true;
      }
      //  inline bool check() const
      //  {
      //    return ( WaitForSingleObject( handle_, 0 ) == WAIT_OBJECT_0 );
      //  }
      inline NativeEventType get() const
      {
        return handle_;
      }
      ~Event()
      {
        if ( handle_ != -1 )
          close( handle_ );
      }
    };

#endif

  }

}
