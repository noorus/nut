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

  //! \class ScopedRWLock
  //! Automation for scoped acquisition and release of an RWLock.
  class ScopedRWLock {
  protected:
    platform::RWLock* lock_;
    bool exclusive_;
    bool locked_;
  public:
    //! Constructor.
    //! \param  lock      The lock to acquire.
    //! \param  exclusive (Optional) true to acquire in exclusive mode, false for shared.
    ScopedRWLock( platform::RWLock* lock, bool exclusive = true ): lock_( lock ), exclusive_( exclusive ), locked_( true )
    {
      exclusive_ ? lock_->lock() : lock_->lockShared();
    }
    //! Call directly if you want to unlock before object leaves scope.
    void unlock()
    {
      if ( locked_ )
        exclusive_ ? lock_->unlock() : lock_->unlockShared();
      locked_ = false;
    }
    ~ScopedRWLock() { unlock(); }
  };

}