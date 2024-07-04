#include "tests/doctest.h"

#include <stdexcept>

#include "chuffed/caching/cache/Threshold.h"
#include "tests/caching/cache/CacheMocks.h"
#include "tests/caching/cache/CacheTests.h"

TEST_CASE( "Threshold tests" ) {
  typedef Threshold<int, MockDomPart> Cache;

  testCache( new Cache( 500, 300 ) );

  SUBCASE( "Constructing the cache with invalid arguments should throw an exception" ) {
    CHECK_THROWS_AS( Cache( 0, 0 ), std::invalid_argument );
    CHECK_THROWS_AS( Cache( -1, 1 ), std::invalid_argument );
    CHECK_THROWS_AS( Cache( 1, -1 ), std::invalid_argument );
    CHECK_THROWS_AS( Cache( 0, 1 ), std::invalid_argument );
    CHECK_THROWS_AS( Cache( 1, 0 ), std::invalid_argument );

    CHECK_NOTHROW( Cache( 1, 1 ) );
  }

  SUBCASE( "Pruning an empty cache should not remove any entries" ) {
    Cache emptyCache( 500, 300 );
    CHECK( emptyCache.insert( ProjectionKey( 1, 1, MockDomPart( 1, 1) ), 600 ) );

    CHECK_FALSE( emptyCache.empty() );
    CHECK( (emptyCache.size() == 1) );
    CHECK( (emptyCache.numKeys() == 1) );
    CHECK( (emptyCache.hits() == 0) );
  }

  Cache cache( 500, 300 );
  cache.insert( ProjectionKey( 1, 100, MockDomPart(5, 7) ), 100 );
  cache.insert( ProjectionKey( 7, 103, MockDomPart(3, 3) ), 150 );
  cache.insert( ProjectionKey( 3, 100, MockDomPart(7, 5) ), 200 );
  cache.insert( ProjectionKey( 2, 101, MockDomPart(5, 7) ), 201 );
  cache.insert( ProjectionKey( 4, 102, MockDomPart(10, 10) ), 300 );
  cache.insert( ProjectionKey( 5, 102, MockDomPart(15, 15) ), 400 );
  cache.insert( ProjectionKey( 6, 102, MockDomPart(1, 50) ), 400 );

  REQUIRE_FALSE( cache.empty() );
  REQUIRE( (cache.size() == 7) );
  REQUIRE( (cache.numKeys() == 4) );
  REQUIRE( (cache.hits() == 0) );

  SUBCASE( "Insertion should trigger pruning only after the specified number of iterations" ) {
    cache.insert( ProjectionKey( 10, 104, MockDomPart(1, 1) ), 500 );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 5) );
    CHECK( (cache.numKeys() == 3) );
    CHECK( (cache.hits() == 0) );

    cache.insert( ProjectionKey( 15, 104, MockDomPart(2, 2) ), 999 );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 6) );
    CHECK( (cache.numKeys() == 3) );
    CHECK( (cache.hits() == 0) );

    cache.insert( ProjectionKey( 15, 105, MockDomPart(2, 2) ), 1300 );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 1) );
    CHECK( (cache.numKeys() == 1) );
    CHECK( (cache.hits() == 0) );

    cache.insert( ProjectionKey( 15, 106, MockDomPart(2, 2) ), 1799 );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 2) );
    CHECK( (cache.numKeys() == 2) );
    CHECK( (cache.hits() == 0) );
  }

  SUBCASE( "Pruning should remove the correct entries" ) {
    CHECK( (cache.find( ProjectionKey( 1, 100, MockDomPart(5, 7) ), 500 ) == std::pair{-1, false}) );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 4) );
    CHECK( (cache.numKeys() == 2) );
    CHECK( (cache.hits() == 0) );

    CHECK( (cache.find( ProjectionKey( 7, 103, MockDomPart(3, 3) ), 500 ) == std::pair{-1, false}) );
    CHECK( (cache.find( ProjectionKey( 1, 100, MockDomPart(5, 7) ), 500 ) == std::pair{-1, false}) );
    CHECK( (cache.find( ProjectionKey( 3, 100, MockDomPart(7, 5) ), 500 ) == std::pair{-1, false}) );
    CHECK( (cache.find( ProjectionKey( 2, 101, MockDomPart(5, 7) ), 500 ) == std::pair{2, true}) );
    CHECK( (cache.find( ProjectionKey( 20, 102, MockDomPart(10, 10) ), 500 ) == std::pair{4, true}) );
  }

  SUBCASE( "A cache hit should update the last hit of an entry" ) {
    CHECK( (cache.find( ProjectionKey( 1, 100, MockDomPart(5, 7) ), 400 ) == std::pair{1, true}) );
    CHECK( (cache.find( ProjectionKey( 7, 103, MockDomPart(3, 3) ), 450 ) == std::pair{7, true}) );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 7) );
    CHECK( (cache.numKeys() == 4) );
    CHECK( (cache.hits() == 2) );

    CHECK( (cache.find( ProjectionKey( 2, 101, MockDomPart(5, 7) ), 550 ) == std::pair{-1, false}) );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 5) );
    CHECK( (cache.numKeys() == 3) );
    CHECK( (cache.hits() == 2) );

    CHECK( (cache.find( ProjectionKey( 1, 100, MockDomPart(5, 7) ), 700 ) == std::pair{1, true}) );
    CHECK( (cache.find( ProjectionKey( 7, 103, MockDomPart(3, 3) ), 700 ) == std::pair{7, true}) );

    CHECK_FALSE( cache.empty() );
    CHECK( (cache.size() == 5) );
    CHECK( (cache.numKeys() == 3) );
    CHECK( (cache.hits() == 4) );
  }

  SUBCASE( "Pruning should not remove entries that do not exceed the threshold" ) {
    Cache smallCache( 500, 300 );
    smallCache.insert( ProjectionKey( 1, 100, MockDomPart( 5, 7) ), 401 );
    smallCache.insert( ProjectionKey( 2, 101, MockDomPart( 5, 7) ), 450 );
    smallCache.insert( ProjectionKey( 3, 100, MockDomPart( 7, 5) ), 600 );

    CHECK_FALSE( smallCache.empty() );
    CHECK( (smallCache.size() == 3) );
    CHECK( (smallCache.numKeys() == 2) );
    CHECK( (smallCache.hits() == 0) );

    CHECK( (smallCache.find( ProjectionKey( 5, 100, MockDomPart( 5, 7) ), 700 ) == std::pair{1, true}) );

    CHECK_FALSE( smallCache.empty() );
    CHECK( (smallCache.size() == 3) );
    CHECK( (smallCache.numKeys() == 2) );
    CHECK( (smallCache.hits() == 1) );
  }

  SUBCASE( "The cache should be empty when all entries exceed the threshold" ) {
    CHECK( (cache.find( ProjectionKey( 1, 100, MockDomPart(5, 7) ), 2000 ) == std::pair{-1, false}) );

    CHECK( cache.empty() );
    CHECK( (cache.size() == 0) );
    CHECK( (cache.numKeys() == 0) );
    CHECK( (cache.hits() == 0) );
  }
}