#include "tests/doctest.h"

#include <memory>
#include <stdexcept>

#include "chuffed/caching/cache/BoundedCache.h"
#include "CacheMocks.h"
#include "CacheTests.h"
#include "tests/caching/cache/policies/MockEvictionPolicy.h"

using std::make_unique;

using ::testing::NiceMock;
using ::testing::InSequence;
using ::testing::Return;

TEST_CASE( "Bounded cache tests" ) {
  typedef BoundedCache<int, MockDomPart> Cache;

  testCache( new Cache(1000, 1, make_unique<NiceMock<MockEvictionPolicy>>()) );

  SUBCASE( "Constructing the bounded cache with invalid arguments should throw an exception" ) {
    CHECK_THROWS_AS( Cache( 0, 0, make_unique<NiceMock<MockEvictionPolicy>>() ), std::invalid_argument );
    CHECK_THROWS_AS( Cache( -1, 1, make_unique<NiceMock<MockEvictionPolicy>>() ), std::invalid_argument );
    CHECK_THROWS_AS( Cache( 1, -1, make_unique<NiceMock<MockEvictionPolicy>>() ), std::invalid_argument );
    CHECK_THROWS_AS( Cache( 0, 1, make_unique<NiceMock<MockEvictionPolicy>>() ), std::invalid_argument );
    CHECK_THROWS_AS( Cache( 1, 0, make_unique<NiceMock<MockEvictionPolicy>>() ), std::invalid_argument );

    CHECK_NOTHROW( Cache( 1, 1, make_unique<NiceMock<MockEvictionPolicy>>() ) );
  }

  SUBCASE( "The policy should be notified after an insertion" ) {
    auto policy = make_unique<MockEvictionPolicy>();

    EXPECT_CALL(*policy, inserted(1)).Times(1);
    EXPECT_CALL(*policy, inserted(2)).Times(1);
    EXPECT_CALL(*policy, inserted(3)).Times(1);

    Cache cache(5, 2, std::move(policy));

    CHECK( cache.insert( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 2, 2, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 3, 1, MockDomPart(0, 2) ), 0 ) );
  }

  SUBCASE( "The policy should be notified on cache hits" ) {
    auto policy = make_unique<MockEvictionPolicy>();

    {
      InSequence seq;

      EXPECT_CALL(*policy, inserted(1)).Times(1);
      EXPECT_CALL(*policy, inserted(2)).Times(1);
      EXPECT_CALL(*policy, inserted(3)).Times(1);

      EXPECT_CALL(*policy, cacheHit(1)).Times(1);
      EXPECT_CALL(*policy, cacheHit(3)).Times(1);
      EXPECT_CALL(*policy, cacheHit(2)).Times(1);
    }

    Cache cache(5, 2, std::move(policy));

    CHECK( cache.insert( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 2, 2, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 3, 1, MockDomPart(0, 2) ), 0 ) );

    CHECK( (cache.find( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) == std::pair<int, bool>{1, true}) );
    CHECK( (cache.find( ProjectionKey( 3, 1, MockDomPart(0, 2) ), 0 ) == std::pair<int, bool>{3, true}) );
    CHECK( (cache.find( ProjectionKey( 2, 2, MockDomPart(0, 0) ), 0 ) == std::pair<int, bool>{2, true}) );

    CHECK( (cache.find( ProjectionKey( 0, 1, MockDomPart(2, 2) ), 0 ) == std::pair<int, bool>{-1, false}) );
  }

  SUBCASE( "Inserting an entry when the cache is full should trigger pruning" ) {
    auto policy = make_unique<MockEvictionPolicy>();

    {
      InSequence seq;
      EXPECT_CALL( *policy, inserted( 1 ) ).Times( 1 );
      EXPECT_CALL( *policy, inserted( 2 ) ).Times( 1 );
      EXPECT_CALL( *policy, inserted( 3 ) ).Times( 1 );
      EXPECT_CALL( *policy, inserted( 4 ) ).Times( 1 );

      EXPECT_CALL( *policy, getCandidate() ).Times( 1 ).WillOnce(Return(2));
      EXPECT_CALL( *policy, erased(2) ).Times( 1 );

      EXPECT_CALL( *policy, getCandidate() ).Times( 1 ).WillOnce(Return(4));
      EXPECT_CALL( *policy, erased(4) ).Times( 1 );

      EXPECT_CALL( *policy, inserted( 5 ) ).Times( 1 );
    }

    Cache cache(4, 2, std::move(policy));

    CHECK( cache.insert( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 2, 2, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 3, 1, MockDomPart(0, 2) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 4, 3, MockDomPart(0, 0) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 5, 4, MockDomPart(0, 0) ), 0 ) );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 3) );
    CHECK( (cache.numKeys() == 2) );
  }

  SUBCASE( "A prune size that is larger than the cache size should remove all entries" ) {
    auto policy = make_unique<MockEvictionPolicy>();

    {
      InSequence seq;
      EXPECT_CALL( *policy, inserted( 1 ) ).Times( 1 );
      EXPECT_CALL( *policy, inserted( 2 ) ).Times( 1 );

      EXPECT_CALL( *policy, getCandidate() ).Times( 1 ).WillOnce(Return(1));
      EXPECT_CALL( *policy, erased(1) ).Times( 1 );

      EXPECT_CALL( *policy, getCandidate() ).Times( 1 ).WillOnce(Return(2));
      EXPECT_CALL( *policy, erased(2) ).Times( 1 );

      EXPECT_CALL( *policy, inserted( 3 ) ).Times( 1 );
    }

    Cache cache(2, 4, std::move(policy));

    CHECK( cache.insert( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 2, 2, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 3, 1, MockDomPart(0, 2) ), 0 ) );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 1) );
    CHECK( (cache.numKeys() == 1) );
  }

  SUBCASE( "Attempting to insert a dominated entry in a full cache should still trigger pruning" ) {
    auto policy = make_unique<MockEvictionPolicy>();

    {
      InSequence seq;
      EXPECT_CALL( *policy, inserted( 1 ) ).Times( 1 );
      EXPECT_CALL( *policy, inserted( 2 ) ).Times( 1 );
      EXPECT_CALL( *policy, inserted( 3 ) ).Times( 1 );

      EXPECT_CALL( *policy, getCandidate() ).Times( 1 ).WillOnce(Return(3));
      EXPECT_CALL( *policy, erased(3) ).Times( 1 );
    }

    Cache cache(3, 1, std::move(policy));

    CHECK( cache.insert( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 2, 2, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 3, 1, MockDomPart(0, 2) ), 0 ) );
    CHECK_FALSE( cache.insert( ProjectionKey( 4, 1, MockDomPart(0, 0) ), 0 ) );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 2) );
    CHECK( (cache.numKeys() == 2) );
  }

  SUBCASE( "Removal should continue until the desired cache size is reached" ) {
    auto policy = make_unique<MockEvictionPolicy>();

    {
      InSequence seq;
      EXPECT_CALL( *policy, inserted( 1 ) ).Times( 1 );
      EXPECT_CALL( *policy, inserted( 2 ) ).Times( 1 );
      EXPECT_CALL( *policy, inserted( 3 ) ).Times( 1 );
      EXPECT_CALL( *policy, inserted( 4 ) ).Times( 1 );

      EXPECT_CALL( *policy, getCandidate() ).Times( 1 ).WillOnce(Return(2));
      EXPECT_CALL( *policy, erased(2) ).Times( 1 );
      EXPECT_CALL( *policy, getCandidate() ).Times( 1 ).WillOnce(Return(-1));
      EXPECT_CALL( *policy, erased(-1) ).Times( 1 );
      EXPECT_CALL( *policy, getCandidate() ).Times( 1 ).WillOnce(Return(8));
      EXPECT_CALL( *policy, erased(8) ).Times( 1 );
      EXPECT_CALL( *policy, getCandidate() ).Times( 1 ).WillOnce(Return(1));
      EXPECT_CALL( *policy, erased(1) ).Times( 1 );

      EXPECT_CALL( *policy, inserted( 5 ) ).Times( 1 );
    }

    Cache cache(4, 2, std::move(policy));

    CHECK( cache.insert( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 2, 2, MockDomPart(1, 1) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 3, 1, MockDomPart(0, 2) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 4, 3, MockDomPart(0, 2) ), 0 ) );
    CHECK( cache.insert( ProjectionKey( 5, 4, MockDomPart(0, 0) ), 0 ) );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 3) );
    CHECK( (cache.numKeys() == 3) );
  }
}