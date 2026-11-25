#pragma once

#include "nut_types.hpp"

namespace nut {

  namespace platform {

#ifdef NUT_PLATFORM_WINDOWS

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

#elif NUT_PLATFORM_LINUX

    //! \class RWLock
    //! Reader-writer lock class for easy portability.
    class RWLock {
    protected:
      mutable pthread_rwlock_t lock_ = PTHREAD_RWLOCK_INITIALIZER;
    public:
      RWLock()
      {
        //if ( pthread_rwlock_init( &lock_, nullptr ) )
        //  NUT_RUNTIME_EXCEPT( "pthread_rwlock_init failed" );
      }
      ~RWLock()
      {
        pthread_rwlock_destroy( &lock_ );
      }
      inline void lock() const
      {
        if ( pthread_rwlock_wrlock( &lock_ ) )
          NUT_RUNTIME_EXCEPT( "pthread_rwlock_wrlock failed" );
      }
      inline void unlock() const
      {
        if ( pthread_rwlock_unlock( &lock_ ) )
          NUT_RUNTIME_EXCEPT( "pthread_rwlock_unlock failed" );
      }
      inline void lockShared() const
      {
        if ( pthread_rwlock_rdlock( &lock_ ) )
          NUT_RUNTIME_EXCEPT( "pthread_rwlock_rdlock failed" );
      }
      inline void unlockShared() const
      {
        unlock();
      }
    };

#endif

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
