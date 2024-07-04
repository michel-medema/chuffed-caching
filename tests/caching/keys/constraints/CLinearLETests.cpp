#include "tests/doctest.h"

#include "chuffed/caching/keys/constraints/CLinearLE.h"
#include "chuffed/caching/keys/constraints/CMinimum.h"

TEST_CASE( "CLinearLE dominance" ) {
  SUBCASE( "Linear constraint" ) {
    CLinearLE c( 1, 10 );

    SUBCASE( "A constraint dominates a more strict constraint" ) {
      CLinearLE c1( c.id, 9 );
      CHECK( c.dominates( c1 ) );
    }

    SUBCASE( "A constraint dominates an equivalent constraint" ) {
      CLinearLE c1( c.id, 10 );
      CHECK( c.dominates( c1 ) );
    }

    SUBCASE( "A constraint does not dominate a less strict constraint" ) {
      CLinearLE c1( c.id, 11 );
      CHECK_FALSE( c.dominates( c1 ) );
    }

    SUBCASE( "Negative constants" ) {
      CHECK_FALSE( CLinearLE( 1, 10 ).dominates( CLinearLE( c.id, 11 ) ) );
      CHECK( CLinearLE( 1, -10 ).dominates( CLinearLE( c.id, -11 ) ) );
    }

    SUBCASE( "Constraints with different identifier" ) {
      CLinearLE c1( 2, 10 );
      CHECK_FALSE( c.dominates( c1 ) );
      CHECK_FALSE( c1.dominates( c ) );
    }

    SUBCASE( "Different type of constraint" ) {
      CMinimum c1( c.id, 5, 10 );
      CHECK_FALSE( c.dominates( c1 ) );
    }
  }

  SUBCASE( "Reified linear constraint" ) {
    CLinearLE c_true( 1, 0, true );
    CLinearLE c_false( 1, 0, false );

    SUBCASE( "A constraint never dominates a constraint with a different boolean value" ) {
      SUBCASE( "More strict constraint" ) {
        CLinearLE c1_true( c_true.id, -1, true );
        CLinearLE c1_false( c_false.id, -1, false );

        CHECK_FALSE( c_true.dominates( c1_false ) );
        CHECK_FALSE( c_false.dominates( c1_true ) );
      }

      SUBCASE( "Equivalent constraint" ) {
        CLinearLE c1_true( c_true.id, 0, true );
        CLinearLE c1_false( c_false.id, 0, false );

        CHECK_FALSE( c_true.dominates( c1_false ) );
        CHECK_FALSE( c_false.dominates( c1_true ) );
      }

      SUBCASE( "Less strict constraint" ) {
        CLinearLE c1_true( c_true.id, 1, true );
        CLinearLE c1_false( c_false.id, 1, false );

        CHECK_FALSE( c_true.dominates( c1_false ) );
        CHECK_FALSE( c_false.dominates( c1_true ) );
      }
    }

    SUBCASE( "Equal boolean values" ) {
      SUBCASE( "A constraint dominates a more strict constraint" ) {
        CLinearLE c1_true( c_true.id, -1, true );
        CLinearLE c1_false( c_false.id, -1, false );

        CHECK( c_true.dominates( c1_true ) );
        CHECK( c_false.dominates( c1_false ) );
      }

      SUBCASE( "A constraint dominates an equivalent constraint" ) {
        CLinearLE c1_true( c_true.id, 0, true );
        CLinearLE c1_false( c_false.id, 0, false );

        CHECK( c_true.dominates( c1_true ) );
        CHECK( c_false.dominates( c1_false ) );
      }

      SUBCASE( "A constraint does not dominate a less strict constraint" ) {
        CLinearLE c1_true( c_true.id, 1, true );
        CLinearLE c1_false( c_false.id, 1, false );

        CHECK_FALSE( c_true.dominates( c1_true ) );
        CHECK_FALSE( c_false.dominates( c1_false ) );
      }
    }

    SUBCASE( "Constraints with different identifier" ) {
      CLinearLE c1( 2, 0, true );
      CHECK_FALSE( c_true.dominates( c1 ) );
      CHECK_FALSE( c1.dominates( c_true ) );
    }

    SUBCASE( "Different type of constraint" ) {
      CMinimum c1( c_true.id, 5, 10 );
      CHECK_FALSE( c_true.dominates( c1 ) );
    }
  }
}