#include "tests/doctest.h"

#include <vector>
#include <unordered_map>
#include <memory>

#include "chuffed/caching/keys/DominancePart.h"
#include "chuffed/caching/keys/constraints/DomConstraintKey.h"
#include "chuffed/caching/keys/constraints/CLinearLE.h"
#include "chuffed/caching/keys/VariableDomain.h"


using std::vector;
using std::unordered_map;
using std::unique_ptr;
using std::make_unique;
using std::move;

vector<unique_ptr<DomConstraintKey>> emptyDomConstraints() {
  return vector<unique_ptr<DomConstraintKey>>{};
}

TEST_CASE( "Dominance Part" ) {
  CLinearLE c1( 1, 3 );
  CLinearLE c2( 1, 7 );
  CLinearLE c3( 1, 1 );
  CLinearLE c4( 1, 5 );

  VariableDomain v1( 1, 3, 3, vector<bool>{true, true, false, true} );
  VariableDomain v2( 2, 0, 3, vector<bool>{true, true, true} );
  VariableDomain v3( 1, 3, 2, vector<bool>{true, false, false, true} );
  VariableDomain v4( 1, 3, 2, vector<bool>{true, true} );
  VariableDomain v5( 1, 4, 2, vector<bool>{true, false, true} );
  VariableDomain v6( 2, 1, 2, vector<bool>{true, true} );
  VariableDomain v7( 3, 10, 3, vector<bool>{true, true, false, true} );

  unordered_map<int, VariableDomain> emptyVariables;

  SUBCASE( "Constraint dominance" ) {
    SUBCASE( "All constraints dominating" ) {
      vector<unique_ptr<DomConstraintKey>> cs1{};
      cs1.push_back( make_unique<CLinearLE>( c1 ) );
      cs1.push_back( make_unique<CLinearLE>( c2 ) );

      DominancePart d( std::move( cs1 ), emptyVariables );

      vector<unique_ptr<DomConstraintKey>> cs2{};
      cs2.push_back( make_unique<CLinearLE>( c3 ) );
      cs2.push_back( make_unique<CLinearLE>( c4 ) );

      DominancePart d1( std::move( cs2 ), emptyVariables );

      CHECK( d.dominates( d1 ) );
    }

    SUBCASE( "Some but not all constraints dominating" ) {
      vector<unique_ptr<DomConstraintKey>> cs1{};
      cs1.push_back( make_unique<CLinearLE>( c1 ) );
      cs1.push_back( make_unique<CLinearLE>( c4 ) );

      DominancePart d( std::move( cs1 ), emptyVariables );

      vector<unique_ptr<DomConstraintKey>> cs2{};
      cs2.push_back( make_unique<CLinearLE>( c3 ) );
      cs2.push_back( make_unique<CLinearLE>( c2 ) );

      DominancePart d1( std::move( cs2 ), emptyVariables );

      CHECK_FALSE( d.dominates( d1 ) );
    }

    SUBCASE( "No constraints dominating" ) {
      vector<unique_ptr<DomConstraintKey>> cs1{};
      cs1.push_back( make_unique<CLinearLE>( c3 ) );
      cs1.push_back( make_unique<CLinearLE>( c4 ) );

      DominancePart d( std::move( cs1 ), emptyVariables );

      vector<unique_ptr<DomConstraintKey>> cs2{};
      cs2.push_back( make_unique<CLinearLE>( c2 ) );
      cs2.push_back( make_unique<CLinearLE>( c2 ) );

      DominancePart d1( std::move( cs2 ), emptyVariables );

      CHECK_FALSE( d.dominates( d1 ) );
    }
  }

  SUBCASE( "Variable dominance" ) {
    SUBCASE( "The same set of variables and all dominating" ) {
      DominancePart d( emptyDomConstraints(), unordered_map<int, VariableDomain>{ {v1.id, v1}, {v2.id, v2} } );
      DominancePart d1( emptyDomConstraints(), unordered_map<int, VariableDomain>{ {v3.id, v3}, {v6.id, v6} } );

      CHECK( d.dominates( d1 ) );
    }

    SUBCASE( "The same dominating variables in a different order" ) {
      DominancePart d( emptyDomConstraints(), unordered_map<int, VariableDomain>{ {v2.id, v2}, {v1.id, v1} } );
      DominancePart d1( emptyDomConstraints(), unordered_map<int, VariableDomain>{ {v3.id, v3}, {v6.id, v6} } );

      CHECK( d.dominates( d1 ) );
    }

    SUBCASE( "Dominance with one variable missing from the domain" ) {
      DominancePart d( emptyDomConstraints(), unordered_map<int, VariableDomain>{{v1.id, v1} } );
      DominancePart d1( emptyDomConstraints(), unordered_map<int, VariableDomain>{{v3.id, v3}, {v6.id, v6} } );

      CHECK( d.dominates( d1 ) );
    }

    SUBCASE( "The same set of variables but not all dominating" ) {
      DominancePart d( emptyDomConstraints(), unordered_map<int, VariableDomain>{{v5.id, v5}, {v2.id, v2} } );
      DominancePart d1( emptyDomConstraints(), unordered_map<int, VariableDomain>{{v1.id, v1}, {v6.id, v6} } );

      CHECK_FALSE( d.dominates( d1 ) );
    }

    SUBCASE( "All variables dominated by missing domains" ) {
      DominancePart d( emptyDomConstraints(), unordered_map<int, VariableDomain>{{v5.id, v5}, {v2.id, v2} } );
      DominancePart d1( emptyDomConstraints(), unordered_map<int, VariableDomain>{} );

      CHECK_FALSE( d.dominates( d1 ) );
    }

    SUBCASE( "Each part contains a variable that is not present in the other part" ) {
      DominancePart d( emptyDomConstraints(), unordered_map<int, VariableDomain>{{v1.id, v1} } );
      DominancePart d1( emptyDomConstraints(), unordered_map<int, VariableDomain>{{v2.id, v2} } );

      CHECK_FALSE( d.dominates( d1 ) );
    }

    SUBCASE( "Domain contains a variable that is not present in the other domain" ) {
      DominancePart d( emptyDomConstraints(), unordered_map<int, VariableDomain>{ {v1.id, v1}, {v2.id, v2}, {v7.id, v7} } );
      DominancePart d1( emptyDomConstraints(), unordered_map<int, VariableDomain>{ {v4.id, v4}, {v6.id, v6} } );

      CHECK_FALSE( d.dominates( d1 ) );
    }
  }

  SUBCASE( "Constraint and variable dominance" ) {
    SUBCASE( "Both constraints and variables dominating" ) {
      vector<unique_ptr<DomConstraintKey>> cs1{};
      cs1.push_back( make_unique<CLinearLE>( c1 ) );

      DominancePart d( std::move( cs1 ), unordered_map<int, VariableDomain>{ {v1.id, v1} } );

      vector<unique_ptr<DomConstraintKey>> cs2{};
      cs2.push_back( make_unique<CLinearLE>( c3 ) );

      DominancePart d1( std::move( cs2 ), unordered_map<int, VariableDomain>{ {v5.id, v5} } );

      CHECK( d.dominates( d1 ) );
    }

    SUBCASE( "Constraints not dominating" ) {
      vector<unique_ptr<DomConstraintKey>> cs1{};
      cs1.push_back( make_unique<CLinearLE>( c1 ) );

      DominancePart d( std::move( cs1 ), unordered_map<int, VariableDomain>{ {v1.id, v1} } );

      vector<unique_ptr<DomConstraintKey>> cs2{};
      cs2.push_back( make_unique<CLinearLE>( c4 ) );

      DominancePart d1( std::move( cs2 ), unordered_map<int, VariableDomain>{ {v5.id, v5} } );

      CHECK_FALSE( d.dominates( d1 ) );
    }

    SUBCASE( "Variables not dominating" ) {
      vector<unique_ptr<DomConstraintKey>> cs1{};
      cs1.push_back( make_unique<CLinearLE>( c1 ) );

      DominancePart d( std::move( cs1 ), unordered_map<int, VariableDomain>{ {v4.id, v4} } );

      vector<unique_ptr<DomConstraintKey>> cs2{};
      cs2.push_back( make_unique<CLinearLE>( c3 ) );

      DominancePart d1( std::move( cs2 ), unordered_map<int, VariableDomain>{ {v1.id, v1} } );

      CHECK_FALSE( d.dominates( d1 ) );
    }

    SUBCASE( "Neither constraints nor variables dominating" ) {
      vector<unique_ptr<DomConstraintKey>> cs1{};
      cs1.push_back( make_unique<CLinearLE>( c2 ) );

      DominancePart d( std::move( cs1 ), unordered_map<int, VariableDomain>{ {v4.id, v4} } );

      vector<unique_ptr<DomConstraintKey>> cs2{};
      cs2.push_back( make_unique<CLinearLE>( c4 ) );

      DominancePart d1( std::move( cs2 ), unordered_map<int, VariableDomain>{} );

      CHECK_FALSE( d.dominates( d1 ) );
    }
  }
}