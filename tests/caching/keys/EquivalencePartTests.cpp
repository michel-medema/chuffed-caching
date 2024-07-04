#include "tests/doctest.h"

#include <vector>

#include "chuffed/caching/keys/EquivalencePart.h"

using std::vector;
using std::pair;
using std::get;

// TODO: Move to header?
std::size_t hash( const EquivalencePart &p ) {
  return std::hash<EquivalencePart>{}( p );
}

// NOTE: Checking the equivalence of the hash is not strictly safe due to hash collisions.
TEST_CASE( "EquivalencePart tests" ) {
  SUBCASE( "Empty" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{}, vector<pair<int, int>>{}, vector<int64_t>{} );
    EquivalencePart q = EquivalencePart( vector<bool>{}, vector<pair<int, int>>{}, vector<int64_t>{} );

    CHECK( (p == q) );
    CHECK( (hash( p ) == hash( q )) );
  }

  SUBCASE( "All parts equal" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );

    CHECK( (p == q) );
    CHECK( (hash( p ) == hash( q )) );
  }

  SUBCASE( "Boolean part different" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, true, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                      {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );

    CHECK_FALSE( (p == q) );
    CHECK_FALSE( (hash( p ) == hash( q )) );
  }

  SUBCASE( "Different Boolean part length" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, false, true}, vector<pair<int, int>>{{0, 5},
                                                                                                 {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );

    CHECK_FALSE( (p == q) );
    CHECK_FALSE( (hash( p ) == hash( q )) );
  }

  SUBCASE( "Domain ranges different" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {2, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );

    CHECK_FALSE( (p == q) );
    CHECK_FALSE( (hash( p ) == hash( q )) );
  }

  SUBCASE( "Different number of domain ranges" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );

    CHECK_FALSE( (p == q) );
    CHECK_FALSE( (hash( p ) == hash( q )) );
  }

  SUBCASE( "Integer part different" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 8, 11} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );

    CHECK_FALSE( (p == q) );
    CHECK_FALSE( (hash( p ) == hash( q )) );
  }

  SUBCASE( "Different integer part length" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 8, 11} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 9, 11} );

    CHECK_FALSE( (p == q) );
    CHECK_FALSE( (hash( p ) == hash( q )) );
  }

  SUBCASE( "Boolean part and domain ranges different" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{1, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, true, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                      {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );

    CHECK_FALSE( (p == q) );
    CHECK_FALSE( (hash( p ) == hash( q )) );
  }

  SUBCASE( "Boolean and integer part different" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, false, false, true}, vector<pair<int, int>>{{0, 5},
                                                                                                        {3, 9}},
                                         vector<int64_t>{1, 7, 9, 11} );

    CHECK_FALSE( (p == q) );
    CHECK_FALSE( (hash( p ) == hash( q )) );
  }

  SUBCASE( "Domain ranges and integer part different" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{1, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{2, 4} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5}},
                                         vector<int64_t>{1, 5, 7} );

    CHECK_FALSE( (p == q) );
    CHECK_FALSE( (hash( p ) == hash( q )) );
  }

  SUBCASE( "Same domain ranges in different order" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{3, 9},
                                                                                                       {0, 5}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );

    CHECK_FALSE( (p == q) );
    CHECK_FALSE( (hash( p ) == hash( q )) );
  }

  SUBCASE( "Same integer values in different order" ) {
    EquivalencePart p = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{1, 5, 7, 9, 11} );
    EquivalencePart q = EquivalencePart( vector<bool>{true, false, true, true}, vector<pair<int, int>>{{0, 5},
                                                                                                       {3, 9}},
                                         vector<int64_t>{5, 11, 7, 1, 9} );

    CHECK_FALSE( (p == q) );
    CHECK_FALSE( (hash( p ) == hash( q )) );
  }
}


/*

vector<unique_ptr<const EqConstraintKey>> emptyConstraints() {
  return vector<unique_ptr<const EqConstraintKey>>{};
}

// NOTE: Checking the equivalence of the hash is not strictly safe due to hash collisions.
TEST_CASE( "EquivalencePart tests" ) {
  VariableDomain v1( 1, 0, vector<bool>{true, true, true, true} );
  VariableDomain v2( 2, 7, vector<bool>{true, false, true, false, true} );
  VariableDomain v3( 3, 20, vector<bool>{true, false, false, true} );
  VariableDomain v4( 4, 1, vector<bool>{true, true, true, true, false, true} );

  auto emptyBooleanConstraints = vector<bool>{};

  SUBCASE( "Empty" ) {
    auto fixedVars = vector<bool>{};
    auto unfixedVars = vector<VariableDomain>{};

    EquivalencePart p = EquivalencePart( fixedVars, unfixedVars, emptyConstraints(), emptyBooleanConstraints );
    EquivalencePart q = EquivalencePart( fixedVars, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

    CHECK( (p == q) );
    CHECK( (hash( p ) == hash( q )) );
  }

  SUBCASE( "No fixed variables" ) {
    auto fixed_vars = std::vector<bool>{false, false, false};
    auto unfixed_vars = std::vector<VariableDomain>{v1, v2, v3};

    EquivalencePart p = EquivalencePart( fixed_vars, unfixed_vars, emptyConstraints(), emptyBooleanConstraints );

    SUBCASE( "The same variables with the same domains" ) {
      EquivalencePart q = EquivalencePart( fixed_vars, unfixed_vars, emptyConstraints(), emptyBooleanConstraints );

      CHECK( (p == q) );
      CHECK( (hash( p ) == hash( q )) );
    }

    SUBCASE( "The same variables but some with different domains" ) {
      VariableDomain v1_q( 1, 0, std::vector<bool>{true, false, true} );
      auto unfixed_q = std::vector<VariableDomain>{v1_q, v2, v3};

      EquivalencePart q = EquivalencePart( fixed_vars, unfixed_q, emptyConstraints(), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }

    SUBCASE( "The domains of two variables exchanged" ) {
      auto unfixed_q = std::vector<VariableDomain>{v1, v3, v2};
      EquivalencePart q = EquivalencePart( fixed_vars, unfixed_q, emptyConstraints(), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }

    SUBCASE( "One less variable" ) {
      auto unfixed_q = std::vector<VariableDomain>{v1, v2};
      EquivalencePart q = EquivalencePart( fixed_vars, unfixed_q, emptyConstraints(), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }
  }

  SUBCASE( "Fixed and unfixed variables" ) {
    auto fixedVars = std::vector<bool>{true, false, true, false, false};
    auto unfixedVars = std::vector<VariableDomain>{v1, v2, v3};

    EquivalencePart p = EquivalencePart( fixedVars, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

    SUBCASE( "The same fixed and unfixed variables" ) {
      EquivalencePart q = EquivalencePart( fixedVars, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

      CHECK( (p == q) );
      CHECK( (hash( p ) == hash( q )) );
    }

    SUBCASE( "The same fixed variables with domains for the unfixed variables exchanged" ) {
      auto unfixed_q = std::vector<VariableDomain>{v3, v2, v1};
      EquivalencePart q = EquivalencePart( fixedVars, unfixed_q, emptyConstraints(), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }

    SUBCASE( "The same fixed variables with different domains for the unfixed variables" ) {
      VariableDomain v1_q( 1, 0, std::vector<bool>{true, false, true} );
      VariableDomain v3_q( 3, 12, std::vector<bool>{true, true, false, true, false, true} );

      auto unfixed_q = std::vector<VariableDomain>{v1_q, v2, v3_q};
      EquivalencePart q = EquivalencePart( fixedVars, unfixed_q, emptyConstraints(), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }

    SUBCASE( "Fewer fixed variables" ) {
      auto fixed_q = std::vector<bool>{true, false, false, false, false};
      auto unfixed_q = std::vector<VariableDomain>{v1, v4, v2, v3};
      EquivalencePart q = EquivalencePart( fixed_q, unfixed_q, emptyConstraints(), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }

    SUBCASE( "More fixed variables" ) {
      auto fixed_q = std::vector<bool>{true, true, true, false, true};
      auto unfixed_q = std::vector<VariableDomain>{v2};
      EquivalencePart q = EquivalencePart( fixed_q, unfixed_q, emptyConstraints(), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }

    SUBCASE( "Fixed variables differ, unfixed variables have same domains in the same order" ) {
      auto fixed_q = {false, true, true, false, false};
      EquivalencePart q = EquivalencePart( fixed_q, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }

    SUBCASE( "Different variables fixed, all unfixed variables have identical domains" ) {
      VariableDomain v1_q( 0, 5, std::vector<bool>{true, true, true} );
      VariableDomain v2_q( 1, 5, std::vector<bool>{true, true, true} );
      VariableDomain v3_q( 2, 5, std::vector<bool>{true, true, true} );
      VariableDomain v4_q( 3, 5, std::vector<bool>{true, true, true} );

      auto fixed_q = {false, true, true, true, false};
      auto unfixed_q = std::vector<VariableDomain>{v1_q, v2_q};

      auto fixed_r = {true, false, true, false, true};
      auto unfixed_r = std::vector<VariableDomain>{v3_q, v4_q};

      EquivalencePart q = EquivalencePart( fixed_q, unfixed_q, emptyConstraints(), emptyBooleanConstraints );
      EquivalencePart r = EquivalencePart( fixed_r, unfixed_r, emptyConstraints(), emptyBooleanConstraints );

      CHECK_FALSE( (q == r) );
      CHECK_FALSE( (hash( q ) == hash( r )) );
    }
  }

  SUBCASE( "All variables fixed" ) {
    auto fixedVars = std::vector<bool>{true, true, true};
    auto unfixedVars = std::vector<VariableDomain>{};

    EquivalencePart p = EquivalencePart( fixedVars, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

    SUBCASE( "The same number of fixed variables" ) {
      EquivalencePart q = EquivalencePart( fixedVars, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

      CHECK( (p == q) );
      CHECK( (hash( p ) == hash( q )) );
    }

    SUBCASE( "A smaller number of variables" ) {
      auto fixed_q = std::vector<bool>{true, true};
      EquivalencePart q = EquivalencePart( fixed_q, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }

    SUBCASE( "A larger number of variables" ) {
      auto fixed_q = std::vector<bool>{true, true, true, true};
      EquivalencePart q = EquivalencePart( fixed_q, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }
  }

  SUBCASE( "Equivalence Constraints" ) {
    auto fixedVars = vector<bool>{};
    auto unfixedVars = vector<VariableDomain>{};

    CBinary c1(1, 5, 8, true);
    CArithmetic c2(2, 3, 8, 12);

    SUBCASE( "Identical keys should have equivalent hashes" ) {
      vector<unique_ptr<const EqConstraintKey>> cs1;
      cs1.push_back( make_unique<CBinary>(c1) );
      cs1.push_back( make_unique<CArithmetic>(c2) );

      vector<unique_ptr<const EqConstraintKey>> cs2;
      cs2.push_back( make_unique<CBinary>(c1) );
      cs2.push_back( make_unique<CArithmetic>(c2) );

      EquivalencePart p( fixedVars, unfixedVars, std::move( cs1 ), emptyBooleanConstraints );
      EquivalencePart q( fixedVars, unfixedVars, std::move( cs2 ), emptyBooleanConstraints );

      CHECK( (p == q) );
      CHECK( (hash( p ) == hash( q )) );
    }

    SUBCASE( "The same keys with different IDs should not be equivalent" ) {
      vector<unique_ptr<const EqConstraintKey>> cs1;
      cs1.push_back( make_unique<CBinary>(c1) );

      vector<unique_ptr<const EqConstraintKey>> cs2;
      cs2.push_back( make_unique<CBinary>( 2, 5, 8, true ) );

      EquivalencePart p( fixedVars, unfixedVars, std::move( cs1 ), emptyBooleanConstraints );
      EquivalencePart q( fixedVars, unfixedVars, std::move( cs2 ), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }

    SUBCASE( "The hash of a key with extremely large values should not change due to overflow" ) {
      vector<unique_ptr<const EqConstraintKey>> cs1;
      cs1.push_back( make_unique<CBinary>( 1, INT64_MAX, INT64_MAX, true ) );

      vector<unique_ptr<const EqConstraintKey>> cs2;
      cs2.push_back( make_unique<CBinary>( 1, INT64_MAX, INT64_MAX, true ) );

      EquivalencePart p( fixedVars, unfixedVars, std::move( cs1 ), emptyBooleanConstraints );
      EquivalencePart q( fixedVars, unfixedVars, std::move( cs2 ), emptyBooleanConstraints );

      CHECK( (p == q) );
      CHECK( (hash( p ) == hash( q )) );


      vector<unique_ptr<const EqConstraintKey>> cs3;
      cs3.push_back( make_unique<CBinary>( 1, INT64_MAX, INT64_MIN, true ) );
      EquivalencePart r( fixedVars, unfixedVars, std::move( cs3 ), emptyBooleanConstraints );

      CHECK_FALSE( (p == r) );
      CHECK_FALSE( (hash( p ) == hash( r )) );
    }

    SUBCASE( "The same keys but in a different order should not be equivalent" ) {
      vector<unique_ptr<const EqConstraintKey>> cs1;
      cs1.push_back( make_unique<CBinary>( c1 ) );
      cs1.push_back( make_unique<CArithmetic>( c2 ) );

      vector<unique_ptr<const EqConstraintKey>> cs2;
      cs2.push_back( make_unique<CArithmetic>( c2 ) );
      cs2.push_back( make_unique<CBinary>( c1 ) );

      EquivalencePart p( fixedVars, unfixedVars, std::move( cs1 ), emptyBooleanConstraints );
      EquivalencePart q( fixedVars, unfixedVars, std::move( cs2 ), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }

    SUBCASE( "Two keys with the same ID but different values should not be equivalent" ) {
      vector<unique_ptr<const EqConstraintKey>> cs1;
      cs1.push_back( make_unique<CBinary>( 2, 5, 8, true ) );

      vector<unique_ptr<const EqConstraintKey>> cs2;
      cs2.push_back( make_unique<CBinary>( 2, 3, 8, true ) );

      EquivalencePart p( fixedVars, unfixedVars, std::move( cs1 ), emptyBooleanConstraints );
      EquivalencePart q( fixedVars, unfixedVars, std::move( cs2 ), emptyBooleanConstraints );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }
  }

  SUBCASE( "Boolean constraints" ) {
    auto fixedVars = vector<bool>{};
    auto unfixedVars = vector<VariableDomain>{};

    SUBCASE( "Identical keys should have equivalent hashes" ) {
      EquivalencePart p( fixedVars, unfixedVars, emptyConstraints(), vector<bool>{true, false, true} );
      EquivalencePart q( fixedVars, unfixedVars, emptyConstraints(), vector<bool>{true, false, true} );

      CHECK( (p == q) );
      CHECK( (hash( p ) == hash( q )) );
    }

    SUBCASE( "Different boolean values" ) {
      EquivalencePart p( fixedVars, unfixedVars, emptyConstraints(), vector<bool>{true, false, true} );
      EquivalencePart q( fixedVars, unfixedVars, emptyConstraints(), vector<bool>{true, true, true} );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }

    SUBCASE( "Different length vectors" ) {
      EquivalencePart p( fixedVars, unfixedVars, emptyConstraints(), vector<bool>{true, false, true} );
      EquivalencePart q( fixedVars, unfixedVars, emptyConstraints(), vector<bool>{true, false, true, true} );

      CHECK_FALSE( (p == q) );
      CHECK_FALSE( (hash( p ) == hash( q )) );
    }
  }

  SUBCASE( "Counting fixed and unfixed variables" ) {
    auto unfixedVars = vector<VariableDomain>{};

    SUBCASE( "No fixed and no unfixed variables" ) {
      EquivalencePart p( vector<bool>{}, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

      CHECK( (p.numFixedVars() == 0) );
      CHECK( (p.numUnfixedVars() == 0) );
    }

    SUBCASE( "All variables unfixed" ) {
      EquivalencePart p( vector<bool>{false, false, false}, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

      CHECK( (p.numFixedVars() == 0) );
      CHECK( (p.numUnfixedVars() == 3) );
    }

    SUBCASE( "A mix of fixed and unfixed variables" ) {
      EquivalencePart p( vector<bool>{true, true, false, false, true}, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

      CHECK( (p.numFixedVars() == 3) );
      CHECK( (p.numUnfixedVars() == 2) );
    }

    SUBCASE( "All variables fixed" ) {
      EquivalencePart p( vector<bool>{true, true}, unfixedVars, emptyConstraints(), emptyBooleanConstraints );

      CHECK( (p.numFixedVars() == 2) );
      CHECK( (p.numUnfixedVars() == 0) );
    }
  }
}
*/