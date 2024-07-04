#include "tests/doctest.h"

#include "chuffed/caching/cache/policies/RandomReplacement.h"

TEST_CASE( "Random Replacement tests" ) {
  RandomReplacement policy;

  SUBCASE( "A policy without any elements should return -1 as the candidate" ) {
    CHECK( (policy.getCandidate() == -1) );
  }

  SUBCASE( "With only one element, the candidate should always be that element" ) {
    policy.inserted( 5 );

    CHECK( (policy.getCandidate() == 5) );
    CHECK( (policy.getCandidate() == 5) );
  }

  SUBCASE( "The insert function should be idempotent" ) {
    policy.inserted( 5 );
    policy.inserted( 5 );

    CHECK( (policy.getCandidate() == 5) );

    policy.erased( 5 );

    CHECK( (policy.getCandidate() == -1) );
  }

  SUBCASE( "The erase function should be idempotent" ) {
    policy.inserted( 1 );

    CHECK( (policy.getCandidate() == 1) );

    policy.erased( 1 );
    CHECK( (policy.getCandidate() == -1) );
    policy.erased( 1 );
    CHECK( (policy.getCandidate() == -1) );
  }

  SUBCASE( "The candidate should be any of the inserted elements" ) {
    policy.inserted( 1 );
    policy.inserted( 5 );
    policy.inserted( 13 );

    CHECK( ( policy.getCandidate() == 1 | 5 | 13 ) );
    CHECK( ( policy.getCandidate() == 1 | 5 | 13 ) );
    CHECK( ( policy.getCandidate() == 1 | 5 | 13 ) );

    policy.erased( 5 );
    CHECK( ( policy.getCandidate() == 1 | 13 ) );
    CHECK( ( policy.getCandidate() == 1 | 13 ) );
    CHECK( ( policy.getCandidate() == 1 | 13 ) );

    policy.erased( 1 );
    CHECK( (policy.getCandidate() == 13) );
    CHECK( (policy.getCandidate() == 13) );
  }

  SUBCASE( "The correct elements should be erased" ) {
    policy.inserted( 3 );
    policy.inserted( 13 );
    policy.inserted( 7 );

    CHECK( ( policy.getCandidate() == 3 | 7 | 13 ) );
    CHECK( ( policy.getCandidate() == 3 | 7 | 13 ) );
    CHECK( ( policy.getCandidate() == 3 | 7 | 13 ) );

    policy.erased( 3 );
    CHECK( ( policy.getCandidate() == 7 | 13 ) );
    CHECK( ( policy.getCandidate() == 7 | 13 ) );
    CHECK( ( policy.getCandidate() == 7 | 13 ) );

    policy.erased( 7 );
    CHECK( (policy.getCandidate() == 13) );
    CHECK( (policy.getCandidate() == 13) );
    CHECK( (policy.getCandidate() == 13) );

    policy.erased( 13 );
    CHECK( (policy.getCandidate() == -1) );
  }
}