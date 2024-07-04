#include "tests/doctest.h"

#include "chuffed/caching/cache/UnboundedCache.h"
#include "CacheMocks.h"
#include "CacheTests.h"

TEST_CASE( "Unbounded cache tests" ) {
  testCache( new UnboundedCache<int, MockDomPart>() );
}