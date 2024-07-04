#include "tests/doctest.h"

#include <vector>

#include "chuffed/caching/keys/VariableDomain.h"
#include "chuffed/vars/int-var.h"

using std::vector;

TEST_CASE( "Variable domain tests" ) {
  SUBCASE( "Equivalence" ) {
    VariableDomain d( 1, 0, 3, vector<bool>{true, true, false, true} );

    SUBCASE( "Equivalent domains" ) {
      CHECK( (d == VariableDomain( 1, 0, 3, vector<bool>{true, true, false, true} )) );
    }

    SUBCASE( "Different identifiers" ) {
      VariableDomain d1( 0, 0, 3, vector<bool>{true, true, false, true} );

      CHECK_FALSE( (d == d1) );
    }

    SUBCASE( "Larger minimum" ) {
      VariableDomain d1( 0, 1, 2, vector<bool>{true, false, true} );

      CHECK_FALSE( (d == d1) );
    }

    SUBCASE( "Lower maximum" ) {
      VariableDomain d1( 1, 0, 2, vector<bool>{true, true} );

      CHECK_FALSE( (d == d1) );
    }

    SUBCASE( "Different values present in domain" ) {
      VariableDomain d1( 1, 0, 2, vector<bool>{true, false, false, true} );

      CHECK_FALSE( (d == d1) );
    }

    SUBCASE( "A different representation of the same logical domain" ) {
      VariableDomain d1( 1, 0, 2, vector<bool>{true, false, false, true, false, false, false} );

      CHECK_FALSE( (d == d1) );
    }
  }

  SUBCASE( "Superset" ) {
    VariableDomain d( 1, 11, 4, vector<bool>{true, true, false, true, true} );

    SUBCASE( "Equal domains" ) {
      VariableDomain d1( 1, 11, 4, vector<bool>{true, true, false, true, true} );

      CHECK( d.dominates( d1 ) );
      CHECK( d1.dominates( d ) );
    }

    SUBCASE( "Domain without minimum value is subset" ) {
      VariableDomain d1( 1, 12, 3, vector<bool>{true, false, true, true} );

      CHECK( d.dominates( d1 ) );
      CHECK_FALSE( d1.dominates( d ) );
    }

    SUBCASE( "Domain without maximum value is subset" ) {
      VariableDomain d1( 1, 11, 3, vector<bool>{true, true, false, true} );

      CHECK( d.dominates( d1 ) );
      CHECK_FALSE( d1.dominates( d ) );
    }

    SUBCASE( "Domain with excluded value is subset" ) {
      VariableDomain d1( 1, 11, 3, vector<bool>{true, true, false, false, true} );

      CHECK( d.dominates( d1 ) );
      CHECK_FALSE( d1.dominates( d ) );
    }

    SUBCASE( "Non-overlapping domains" ) {
      VariableDomain d1( 0, 5, 6, vector<bool>{true, true, true, true, true, true} );

      CHECK_FALSE( d.dominates( d1 ) );
      CHECK_FALSE( d1.dominates( d ) );
    }

    SUBCASE( "Domains contain value not present in the other domain" ) {
      VariableDomain d1( 1, 11, 4, vector<bool>{true, false, true, true, true} );

      CHECK_FALSE( d.dominates( d1 ) );
      CHECK_FALSE( d1.dominates( d ) );
    }
  }

  SUBCASE( "Hash code" ) {
    VariableDomain d1( 1, 0, 3, vector<bool>{true, true, false, true} );
    VariableDomain d2( 1, 0, 3, vector<bool>{true, true, false, true} );
    VariableDomain d3( 2, 0, 3, vector<bool>{true, true, false, true} );
    VariableDomain d4( 1, 1, 3, vector<bool>{true, true, false, true} );
    VariableDomain d5( 1, 0, 2, vector<bool>{true, false, false, true} );
    VariableDomain d6( 1, 0, 3, vector<bool>{true, true, true} );
    VariableDomain d7( 1, 5, 2, vector<bool>{true, false, true} );

    CHECK( ( d1.hash() == d2.hash() ) );
    CHECK_FALSE( ( d1.hash() == d3.hash() ) );
    CHECK_FALSE( ( d1.hash() == d4.hash() ) );
    CHECK_FALSE( ( d1.hash() == d5.hash() ) );
    CHECK_FALSE( ( d1.hash() == d6.hash() ) );
    CHECK_FALSE( ( d1.hash() == d7.hash() ) );
    CHECK_FALSE( ( d4.hash() == d5.hash() ) );
    CHECK_FALSE( ( d4.hash() == d6.hash() ) );
    CHECK_FALSE( ( d4.hash() == d7.hash() ) );
    CHECK_FALSE( ( d5.hash() == d6.hash() ) );
    CHECK_FALSE( ( d5.hash() == d7.hash() ) );
    CHECK_FALSE( ( d6.hash() == d7.hash() ) );
  }

  SUBCASE( "Domain construction" ) {
    IntVar *v = newIntVar( 0, 5 );
    v->initVals();

    SUBCASE( "Complete domain" ) {
      VariableDomain d = VariableDomain::getDomain( v );

      CHECK( (d.id == v->var_id) );
      CHECK( (d.min == 0) );
      CHECK( (d.getValues() == vector<bool>{true, true, true, true, true, true}) );
    }

    SUBCASE( "Values removed from the middle of the domain" ) {
      v->remVal( 3 );

      VariableDomain d = VariableDomain::getDomain( v );

      CHECK( (d.id == v->var_id) );
      CHECK( (d.min == 0) );
      CHECK( (d.getValues() == vector<bool>{true, true, true, false, true, true}) );
    }

    SUBCASE( "Values removed from the beginning of the domain" ) {
      v->remVal( 0 );

      VariableDomain d = VariableDomain::getDomain( v );

      CHECK( (d.id == v->var_id) );
      CHECK( (d.min == 1) );
      CHECK( (d.getValues() == vector<bool>{true, true, true, true, true}) );
    }
  }

  SUBCASE( "Detect complete domain" ) {
    SUBCASE( "Domain with all values between min and max" ) {
      IntVar *v = newIntVar( 1, 7 );
      v->initVals();

      CHECK( VariableDomain::completeDomain( v ) );
    }

    SUBCASE( "Domain with a value removed" ) {
      IntVar *v = newIntVar( 1, 7 );
      v->initVals();
      v->remVal( 2 );

      CHECK_FALSE( VariableDomain::completeDomain( v ) );
    }
  }

  SUBCASE( "Detect original domain" ) {
    SUBCASE( "Domain with all values between original min and max" ) {
      IntVar *v = newIntVar( 1, 7 );
      v->initVals();

      CHECK( VariableDomain::originalDomain( v ) );
    }

    SUBCASE( "Domain with all values but larger minimum" ) {
      IntVar *v = newIntVar( 1, 7 );
      v->initVals();
      v->remVal( 1 );

      CHECK_FALSE( VariableDomain::originalDomain( v ) );
    }

    SUBCASE( "Domain with all values but lower maximum" ) {
      IntVar *v = newIntVar( 1, 7 );
      v->initVals();
      v->remVal( 7 );

      CHECK_FALSE( VariableDomain::originalDomain( v ) );
    }

    SUBCASE( "Domain with a value removed" ) {
      IntVar *v = newIntVar( 1, 7 );
      v->initVals();
      v->remVal( 2 );

      CHECK_FALSE( VariableDomain::originalDomain( v ) );
    }
  }
}