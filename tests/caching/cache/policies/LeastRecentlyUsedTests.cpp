#include "tests/doctest.h"

#include "chuffed/caching/cache/policies/LeastRecentlyUsed.h"

TEST_CASE( "Least recently used tests" ) {
  SUBCASE( "A policy without any elements should return -1 as the candidate" ) {
    LeastRecentlyUsed policy;

    CHECK( (policy.getCandidate() == -1) );
  }

  SUBCASE( "The candidate should be the first element that was inserted" ) {
    LeastRecentlyUsed policy;
    policy.inserted( 1 );
    policy.inserted( 2 );

    CHECK( (policy.getCandidate() == 1) );
  }

  LeastRecentlyUsed policy;

  policy.inserted( 1 );
  policy.inserted( 2 );
  policy.inserted( 3 );

  REQUIRE( (policy.getCandidate() == 1) );

  SUBCASE( "The policy should return a new candidate after erasing the current one" ) {
    policy.erased( 1 );
    CHECK( (policy.getCandidate() == 2) );
    policy.erased( 2 );
    CHECK( (policy.getCandidate() == 3) );
  }

  SUBCASE( "Erasing an element other than the candidate should not change the current candidate" ) {
    CHECK( (policy.getCandidate() == 1) );

    policy.erased( 2 );
    CHECK( (policy.getCandidate() == 1) );

    policy.erased( 1 );
    CHECK( (policy.getCandidate() == 3) );
  }

  SUBCASE( "The erase function should be idempotent" ) {
    policy.erased( 1 );
    CHECK( (policy.getCandidate() == 2) );
    policy.erased( 1 );
    CHECK( (policy.getCandidate() == 2) );
  }

  SUBCASE( "Inserting an existing element should make it the most recently used element" ) {
    policy.inserted( 1 );
    CHECK( (policy.getCandidate() == 2) );

    policy.erased( 2 );
    CHECK( (policy.getCandidate() == 3) );
    policy.erased( 3 );
    CHECK( (policy.getCandidate() == 1) );
  }

  SUBCASE( "A cache hit should make the corresponding element the most recently used element" ) {
    policy.cacheHit( 1 );

    CHECK( (policy.getCandidate() == 2) );

    policy.cacheHit( 3 );

    CHECK( (policy.getCandidate() == 2) );

    policy.cacheHit( 2 );

    CHECK( (policy.getCandidate() == 1) );
  }

  SUBCASE( "A single element with multiple cache hits should remain the most recently used element" ) {
    policy.cacheHit( 1 );
    policy.cacheHit( 1 );
    policy.cacheHit( 1 );

    CHECK( (policy.getCandidate() == 2) );

    policy.cacheHit( 2 );
    policy.cacheHit( 3 );

    CHECK( (policy.getCandidate() == 1) );
  }

  SUBCASE( "Cache hits should make the corresponding elements the most recently used ones" ) {
    policy.cacheHit( 3 );
    policy.cacheHit( 2 );
    policy.cacheHit( 1 );

    CHECK( (policy.getCandidate() == 3) );

    policy.erased( 3 );

    CHECK( (policy.getCandidate() == 2) );

    policy.erased( 2 );

    CHECK( (policy.getCandidate() == 1) );
  }

  SUBCASE( "Inserting a new element should make it the most recently used element" ) {
    policy.cacheHit( 1 );

    CHECK( (policy.getCandidate() == 2) );

    policy.inserted( 4 );

    CHECK( (policy.getCandidate() == 2) );

    policy.erased( 2 );

    CHECK( (policy.getCandidate() == 3) );

    policy.erased( 3 );

    CHECK( (policy.getCandidate() == 1) );

    policy.erased( 1 );

    CHECK( (policy.getCandidate() == 4) );
  }
}