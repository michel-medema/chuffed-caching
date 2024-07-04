#include "tests/doctest.h"

#include <stdexcept>

#include "chuffed/caching/cache/EventCache.h"
#include "tests/caching/cache/CacheMocks.h"

TEST_CASE( "Event cache tests" ) {
  typedef EventCache<int, MockDomPart> Cache;

  // TODO: Do not use absolute path.
  Cache cache( R"(C:\Users\User\CLionProjects\msc-thesis-s3212335-breeman-luc\chuffed\tests\caching\cache\test_events.csv)" );

  SUBCASE( "A key is only inserted if it has at least one hit event" ) {
    CHECK( cache.insert( ProjectionKey(1, 11, MockDomPart(1, 0) ), 0 ) );
    CHECK( cache.insert( ProjectionKey(2, 12, MockDomPart(0, 0) ), 0 ) );
    CHECK( cache.insert( ProjectionKey(3, 11, MockDomPart(0, 1) ), 0 ) );
    CHECK_FALSE( cache.insert( ProjectionKey(4, 13, MockDomPart(0, 0) ), 0 ) );
    CHECK_FALSE( cache.insert( ProjectionKey(5, 14, MockDomPart(0, 0) ), 0 ) );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 3) );
    CHECK( (cache.numKeys() == 2) );
    CHECK( (cache.hits() == 0) );
  }

  SUBCASE( "An entry should be removed if the number of hits reaches the expected number of hits" ) {
    cache.insert( ProjectionKey(1, 11, MockDomPart(1, 0) ), 0 );
    cache.insert( ProjectionKey(2, 12, MockDomPart(0, 0) ), 0 );
    cache.insert( ProjectionKey(3, 11, MockDomPart(0, 1) ), 0 );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 3) );
    CHECK( (cache.numKeys() == 2) );
    CHECK( (cache.hits() == 0) );

    CHECK( (cache.find( ProjectionKey(2, 11, MockDomPart(1, 0) ), 0 ) == std::pair{1, true}) );
    CHECK( (cache.find( ProjectionKey(6, 12, MockDomPart(0, 0) ), 0 ) == std::pair{2, true}) );
    CHECK( (cache.find( ProjectionKey(9, 11, MockDomPart(0, 1) ), 0 ) == std::pair{3, true}) );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 2) );
    CHECK( (cache.numKeys() == 2) );
    CHECK( (cache.hits() == 3) );

    CHECK( (cache.find( ProjectionKey(7, 12, MockDomPart(0, 0) ), 0 ) == std::pair{2, true}) );
    CHECK( (cache.find( ProjectionKey(10, 12, MockDomPart(0, 0) ), 0 ) == std::pair{2, true}) );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 1) );
    CHECK( (cache.numKeys() == 1) );
    CHECK( (cache.hits() == 5) );
  }
}