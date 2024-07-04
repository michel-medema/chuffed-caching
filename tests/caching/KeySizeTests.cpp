#include "tests/doctest.h"

#include <vector>
#include <memory>

#include "chuffed/caching/key-size.h"
#include "chuffed/caching/keys/VariableDomain.h"

using std::vector;
using std::pair;
using std::get;
using std::move;

// NOTE: Checking the equivalence of the hash is not strictly safe due to hash collisions.
TEST_CASE( "Key size tests" ) {
  SUBCASE("Num bytes") {
    SUBCASE( "Empty vector" ) {
      CHECK( (numBytes( vector<bool>{} ) == 0) );
    }

    SUBCASE( "Length multiple of eight" ) {
      vector<bool> v1(8, true);
      CHECK( (numBytes( v1 ) == ceil( v1.capacity() / 8.0 ) ) );

      vector<bool> v2(24, false);
      CHECK( (numBytes( v2 ) == ceil( v2.capacity() / 8.0 ) ) );
    }

    SUBCASE( "Length not a multiple of eight" ) {
      vector<bool> v1(3, true);
      CHECK( (numBytes( v1 ) == ceil( v1.capacity() / 8.0 ) ) );

      vector<bool> v2(100, false);
      CHECK( (numBytes( v2 ) == ceil( v2.capacity() / 8.0 ) ) );
    }
  }
}