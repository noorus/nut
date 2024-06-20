#pragma once

#include "nut_types.hpp"

namespace nut {

  namespace platform {

    #pragma warning( push )
    #pragma warning( disable : 26110 )

    //! \class RWLock
    //! Reader-writer lock class for easy portability.
    class RWLock {
    protected:
      SRWLOCK lock_ {};
    public:
      RWLock() { InitializeSRWLock( &lock_ ); }
      inline void lock() { AcquireSRWLockExclusive( &lock_ ); }
      inline void unlock() { ReleaseSRWLockExclusive( &lock_ ); }
      inline void lockShared() { AcquireSRWLockShared( &lock_ ); }
      inline void unlockShared() { ReleaseSRWLockShared( &lock_ ); }
    };

    #pragma warning( pop )

  }

}