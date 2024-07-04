#include "tests/doctest.h"

#include <climits>

#include "chuffed/caching/keys/constraints/CLinearLE.h"
#include "chuffed/caching/keys/constraints/CMinimum.h"

TEST_CASE("CMinimum dominance") {
    int id = 2;

    SUBCASE("Equivalence") {
        CHECK( CMinimum(id, 0, 10).dominates( CMinimum(id, 0, 10) ) );
        CHECK( CMinimum(id, INT_MAX, INT_MAX).dominates( CMinimum(id, INT_MAX, INT_MAX) ) );
    }

    SUBCASE("Smaller lower bound") {
        CHECK( CMinimum(id, -1, 10).dominates( CMinimum(id, 0, 10) ) );
    }

    SUBCASE("Larger upper bound") {
        CHECK( CMinimum(id, 0, 11).dominates( CMinimum(id, 0, 10) ) );
        CHECK( CMinimum(id, 0, INT_MAX).dominates( CMinimum(id, 0, 10) ) );
    }

    SUBCASE("Smaller lower bound and larger upper bound") {
        CHECK( CMinimum(id, -1, 11).dominates( CMinimum(id, 0, 10) ) );
        CHECK( CMinimum(id, -1, INT_MAX).dominates( CMinimum(id, 0, 10) ) );
    }

    SUBCASE("Larger lower bound") {
        CHECK_FALSE( CMinimum(id, 1, 10).dominates( CMinimum(id, 0, 10) ) );
    }

    SUBCASE("Smaller upper bound") {
        CHECK_FALSE( CMinimum(id, 0, 9).dominates( CMinimum(id, 0, 10) ) );
    }

    SUBCASE("Larger upper and lower bound") {
        CHECK_FALSE( CMinimum(id, 1, 11).dominates( CMinimum(id, 0, 10) ) );
    }

    SUBCASE("Smaller upper and lower bound") {
        CHECK_FALSE( CMinimum(id, -1, 9).dominates( CMinimum(id, 0, 10) ) );
    }

    SUBCASE("Larger lower bound and smaller upper bound") {
        CHECK_FALSE( CMinimum(id, 1, 9).dominates( CMinimum(id, 0, 10) ) );
    }

    SUBCASE("Lower bound higher than upper bound") {
        CHECK( CMinimum(id, 9, 10).dominates( CMinimum(id, 10, 9) ) );
    }

    SUBCASE("Constraints with different identifier") {
        CHECK_FALSE( CMinimum(1, 0, 10).dominates( CMinimum(2, 0, 10) ) );
    }

    SUBCASE("Different type of constraint") {
      CHECK_FALSE( CMinimum(id, 0, 0).dominates(CLinearLE(id, 0) ) );
    }
}