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
      inline void lock() const { AcquireSRWLockExclusive( const_cast<SRWLOCK*>( &lock_ ) ); }
      inline void unlock() const { ReleaseSRWLockExclusive( const_cast<SRWLOCK*>( &lock_ ) ); }
      inline void lockShared() const { AcquireSRWLockShared( const_cast<SRWLOCK*>( &lock_ ) ); }
      inline void unlockShared() const { ReleaseSRWLockShared( const_cast<SRWLOCK*>( &lock_ ) ); }
    };

    #pragma warning( pop )

  }

  //! \class ScopedRWLock
  //! Automation for scoped acquisition and release of an RWLock.
  class ScopedRWLock {
  protected:
    const platform::RWLock* lock_;
    bool exclusive_;
    bool locked_ = true;
  private:
    // No copying or assignment
    ScopedRWLock( ScopedRWLock const & ) = delete;
    ScopedRWLock& operator=( ScopedRWLock const & ) = delete;
  public:
    //! Constructor.
    //! \param  lock      The lock to acquire.
    //! \param  exclusive (Optional) true to acquire in exclusive mode, false for shared.
    explicit ScopedRWLock( const platform::RWLock* lock, bool exclusive = true ): lock_( lock ), exclusive_( exclusive )
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