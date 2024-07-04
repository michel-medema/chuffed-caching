#ifndef CHUFFED_CACHEMOCKS_H
#define CHUFFED_CACHEMOCKS_H

#include "tests/doctest.h"

#include "chuffed/caching/cache/ICache.h"

class MockDomPart {
  public:
    const int a;
    const int b;

    MockDomPart(int a, int b): a(a), b(b) {}

    bool dominates(const MockDomPart& p) const {
      return a >= p.a && b >= p.b;
    }

    std::tuple<size_t, size_t, size_t> size() const {
      return {0, 0, 0};
    }
};

template<>
struct std::hash<const int> {
  std::size_t operator()( const int &i ) const noexcept {
    return std::hash<int>{}(i);
  }
};

#endif //CHUFFED_CACHEMOCKS_H
