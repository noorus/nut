#pragma once

#include "nut_types.hpp"

namespace nut {

  namespace platform {

    //! \class RandomGenerator
    //! Interface into platform-native cryptographically secure pseudo-random generator.
    class RandomGenerator {
    private:
      HCRYPTPROV provider_ = 0;
    public:
      RandomGenerator()
      {
        if ( !CryptAcquireContextA( &provider_, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) )
          NUT_RUNTIME_EXCEPT( "CryptAcquireContextA failed" );
      }
      template <typename Type>
      Type gen()
      {
        Type ret = 0;

        if ( !CryptGenRandom( provider_, sizeof( Type ), reinterpret_cast<BYTE*>( &ret ) ) )
          NUT_RUNTIME_EXCEPT( "CryptGenRandom failed" );

        return ret;
      }
      ~RandomGenerator()
      {
        if ( provider_ )
          CryptReleaseContext( provider_, 0 );
      }
    };

    //! \class RandomSeedSequence
    //! STL-compatible seed sequence adapter for the RandomGenerator class.
    class RandomSeedSequence {
    protected:
      RandomGenerator generator_;
      size_t size_ = 0;
    public:
      RandomSeedSequence(): size_( 0 ) {}
      explicit RandomSeedSequence( size_t size ): size_( size ) {}
      template <typename RandomIt>
      void generate( RandomIt begin, RandomIt end )
      {
        while ( begin != end )
          *begin++ = generator_.gen<uint32_t>();
      }
      size_t size() const noexcept { return size_; }
      template <typename OutputIt>
      void param( OutputIt dest ) const
      {
        for ( auto n = size_; n--; )
          *dest++ = generator_.gen<uint32_t>();
      }
    };

  }

}