#include "tests/doctest.h"

#include "chuffed/caching/cache/policies/FIFO.h"

TEST_CASE( "FIFO tests" ) {
  SUBCASE( "A policy without any elements should return -1 as the candidate" ) {
    FIFO policy;

    CHECK( (policy.getCandidate() == -1) );
  }

  SUBCASE( "The candidate should be the first element that was inserted" ) {
    FIFO policy;

    policy.inserted( 5 );
    policy.inserted( 2 );

    CHECK( (policy.getCandidate() == 5) );
  }

  FIFO policy;

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

  SUBCASE( "Inserting an existing element should put it at the back of the queue" ) {
    policy.inserted( 1 );
    CHECK( (policy.getCandidate() == 2) );

    policy.erased( 2 );
    CHECK( (policy.getCandidate() == 3) );
    policy.erased( 3 );
    CHECK( (policy.getCandidate() == 1) );
  }

  SUBCASE( "New elements should be placed at the back of the queue" ) {
    policy.erased( 1 );
    CHECK( (policy.getCandidate() == 2) );

    policy.inserted( 10 );
    CHECK( (policy.getCandidate() == 2) );

    policy.inserted( 7 );
    CHECK( (policy.getCandidate() == 2) );

    policy.erased( 2 );
    CHECK( (policy.getCandidate() == 3) );

    policy.erased( 3 );
    CHECK( (policy.getCandidate() == 10) );

    policy.erased( 10 );
    CHECK( (policy.getCandidate() == 7) );

    policy.erased( 7 );
    CHECK( (policy.getCandidate() == -1) );
  }
}