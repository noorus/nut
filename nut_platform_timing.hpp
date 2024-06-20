#pragma once

#include "nut_types.hpp"

namespace nut {

  namespace platform {

    inline void sleep( uint32_t milliseconds )
    {
      ::SleepEx( milliseconds, TRUE );
    }

    inline int64_t unixTimestamp()
    {
      constexpr int64_t c_unixTimeStart = 0x019DB1DED53E8000;
      constexpr int64_t c_ticksPerSecond = 10000000;

      FILETIME ft;
      GetSystemTimeAsFileTime( &ft );

      LARGE_INTEGER li = { .u { .LowPart = ft.dwLowDateTime, .HighPart = static_cast<LONG>( ft.dwHighDateTime ) } };

      return ( li.QuadPart - c_unixTimeStart ) / c_ticksPerSecond;
    }

    //! \class PerformanceTimer
    //! Native performance timer implementation for high-precision timing of tasks.
    class PerformanceTimer {
    private:
      LARGE_INTEGER frequency_;
      LARGE_INTEGER time_;
    public:
      PerformanceTimer(): frequency_ { { 0, 0 } }, time_ { { 0, 0 } }
      {
        if ( !QueryPerformanceFrequency( &frequency_ ) )
          NUT_RUNTIME_EXCEPT( "Couldn't query HPC frequency" );
      }
      void start()
      {
        QueryPerformanceCounter( &time_ );
      }
      double stop()
      {
        LARGE_INTEGER newTime;
        QueryPerformanceCounter( &newTime );
        auto delta = ( newTime.QuadPart - time_.QuadPart ) * 1000000;
        delta /= frequency_.QuadPart;
        double ms = (double)delta / 1000.0;
        return ms;
      }
    };

    //! \class PerformanceClock
    //! Native performance clock implementation for high-precision loop delta timing.
    class PerformanceClock {
    private:
      LARGE_INTEGER current_;
      double frequency_ = 0.0;
      double inverse_ = 0.0;
    public:
      PerformanceClock(): current_ { { 0, 0 } }
      {
        LARGE_INTEGER frequency;
        if ( !QueryPerformanceFrequency( &frequency ) )
          NUT_RUNTIME_EXCEPT( "Couldn't query HPC frequency" );
        frequency_ = static_cast<double>( frequency.QuadPart );
        inverse_ = ( 1000000.0 / frequency.QuadPart );
      }
      //! Call this once before entering whatever loop is going to then keep calling update()
      inline void init()
      {
        QueryPerformanceCounter( &current_ );
      }
      //! Returns the time in seconds since the last call to update()
      inline double update()
      {
        LARGE_INTEGER new_;
        QueryPerformanceCounter( &new_ );
        auto delta = ( new_.QuadPart - current_.QuadPart );
        current_ = new_;
        return ( static_cast<double>( delta ) / frequency_ );
      }
      //! Returns the time in seconds since the last call to update() without "resetting" the counter
      inline double peek() const
      {
        LARGE_INTEGER new_;
        QueryPerformanceCounter( &new_ );
        auto delta = ( new_.QuadPart - current_.QuadPart );
        return ( static_cast<double>( delta ) / frequency_ );
      }
      //! Returns the time in microseconds since the last call to update() without "resetting" the counter
      inline uint64_t peekMicroseconds() const
      {
        LARGE_INTEGER new_;
        QueryPerformanceCounter( &new_ );
        auto delta = ( new_.QuadPart - current_.QuadPart );
        return static_cast<uint64_t>( delta * inverse_ );
      }
      //! Returns the time in milliseconds since the last call to update() without "resetting" the counter
      inline double peekMilliseconds() const
      {
        return static_cast<double>( peekMicroseconds() ) * 0.001;
      }
    };

  }

}