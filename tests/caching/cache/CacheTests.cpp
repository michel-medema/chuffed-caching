#include "CacheTests.h"

void testCache( ICache<int, MockDomPart> *cache ) {
  SUBCASE( "A cache without any entries should be empty" ) {
    CHECK( cache->empty() );
    CHECK( (cache->size() == 0) );
    CHECK( (cache->numKeys() == 0) );
    CHECK( (cache->hits() == 0) );
  }

  SUBCASE( "Keys with unique equivalent parts should be inserted" ) {
    CHECK( cache->insert( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 1) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );

    CHECK( cache->insert( ProjectionKey( 2, 2, MockDomPart(1, 1) ), 0 ) );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 2) );
    CHECK( (cache->numKeys() == 2) );
    CHECK( (cache->hits() == 0) );
  }

  SUBCASE( "An existing key should not be inserted again" ) {
    CHECK( cache->insert( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 1) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );

    CHECK_FALSE( cache->insert( ProjectionKey( 2, 1, MockDomPart(1, 1) ), 0 ) );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 1) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );
  }

  SUBCASE( "Keys with identical equivalent parts but incomparable dominance parts should be inserted" ) {
    CHECK( cache->insert( ProjectionKey( 1, 1, MockDomPart(1, 2) ), 0 ) );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 1) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );

    CHECK( cache->insert( ProjectionKey( 2, 1, MockDomPart(2, 1) ), 0 ) );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 2) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );
  }

  SUBCASE( "A key dominated by an existing entry should not be inserted" ) {
    CHECK( cache->insert( ProjectionKey( 1, 1, MockDomPart(2, 2) ), 0 ) );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 1) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );

    CHECK_FALSE( cache->insert( ProjectionKey( 2, 1, MockDomPart(1, 1) ), 0 ) );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 1) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );
  }

  SUBCASE( "A key dominating an existing entry should be inserted along the existing entries" ) {
    CHECK( cache->insert( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 1) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );

    CHECK( cache->insert( ProjectionKey( 2, 1, MockDomPart(2, 2) ), 0 ) );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 2) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );
  }

  SUBCASE( "A cache without any entries should not produce any matches" ) {
    CHECK( (cache->find( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) == std::pair{-1, false} ) );
    CHECK( (cache->hits() == 0) );
  }

  SUBCASE( "Searching for an identical key should produce a match" ) {
    cache->insert( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 );
    cache->insert( ProjectionKey( 2, 2, MockDomPart(1, 0) ), 0 );
    cache->insert( ProjectionKey( 3, 2, MockDomPart(0, 1) ), 0 );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 3) );
    CHECK( (cache->numKeys() == 2) );
    CHECK( (cache->hits() == 0) );

    CHECK( (cache->find( ProjectionKey( 1, 1, MockDomPart(1, 1) ), 0 ) == std::pair{1, true} ) );
    CHECK( (cache->hits() == 1) );

    CHECK( (cache->find( ProjectionKey( 5, 2, MockDomPart(0, 1) ), 0 ) == std::pair{3, true} ) );
    CHECK( (cache->hits() == 2) );
  }

  SUBCASE( "A key dominated by an entry should produce a match" ) {
    cache->insert( ProjectionKey( 1, 1, MockDomPart(1, 3) ), 0 );
    cache->insert( ProjectionKey( 2, 1, MockDomPart(5, 2) ), 0 );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 2) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );

    // It may match either of the two entries.
    CHECK( cache->find( ProjectionKey( 10, 1, MockDomPart(0, 0) ), 0 ).second );
    CHECK( (cache->hits() == 1) );

    CHECK( (cache->find( ProjectionKey( 5, 1, MockDomPart(2, 2) ), 0 ) == std::pair{2, true} ) );
    CHECK( (cache->hits() == 2) );
  }

  SUBCASE( "A key with matching equivalence part but incomparable dominance part should not produce a match" ) {
    cache->insert( ProjectionKey( 5, 1, MockDomPart(5, 2) ), 0 );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 1) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );

    // It may match either of the two entries.
    CHECK( (cache->find( ProjectionKey( 10, 1, MockDomPart(10, 1) ), 0 ) == std::pair{-1, false} ) );
    CHECK( (cache->hits() == 0) );
  }

  SUBCASE( "A key with matching equivalence part but dominating dominance part should not produce a match" ) {
    cache->insert( ProjectionKey( 5, 1, MockDomPart(5, 2) ), 0 );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 1) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );

    // It may match either of the two entries.
    CHECK( (cache->find( ProjectionKey( 10, 1, MockDomPart(10, 10) ), 0 ) == std::pair{-1, false} ) );
    CHECK( (cache->hits() == 0) );
  }

  SUBCASE( "A key with an identical dominance part but different equivalence part should not produce a match" ) {
    cache->insert( ProjectionKey( 5, 1, MockDomPart(5, 2) ), 0 );

    CHECK_FALSE( cache->empty() );
    CHECK( (cache->size() == 1) );
    CHECK( (cache->numKeys() == 1) );
    CHECK( (cache->hits() == 0) );

    // It may match either of the two entries.
    CHECK( (cache->find( ProjectionKey( 10, 2, MockDomPart(5, 2) ), 0 ) == std::pair{-1, false} ) );
    CHECK( (cache->hits() == 0) );
  }
}
