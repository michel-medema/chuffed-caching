#ifndef CHUFFED_EQUIVALENCEPART_H
#define CHUFFED_EQUIVALENCEPART_H

#include <cstdint>
#include <vector>
#include <algorithm>
#include <memory>
#include <utility>

#include "chuffed/caching/hash.h"
#include "chuffed/caching/key-size.h"
#include "VariableDomain.h"

class EquivalencePart {
  private:
		/*
		 * The equivalence part stores most of the information, such as variable domains and constraint key,
		 * in an integer vector and a boolean vector. This representation is more space efficient than creating
		 * separate objects for each of these components of the equivalence part, because vectors have a small
		 * amount of overhead and creating many vectors results in a large amount of overhead.
		 */
    std::vector<int64_t> intRep;
    std::vector<std::pair<int, int>> unfixedVarBounds;
    std::vector<bool> boolRep;

    std::size_t hashValue() const {
      size_t hash = 0;

      hashCombine( hash, boolRep );

      for ( const std::pair<int, int> &range: unfixedVarBounds) {
        hashCombine( hash, range.first );
        hashCombine( hash, range.second );
      }

      for ( const int64_t v: intRep ) {
        hashCombine( hash, v );
      }

      return hash;
    }

  public:
    const std::size_t hash_;

    EquivalencePart( std::vector<bool> boolRep,
                     std::vector<std::pair<int, int>> unfixedVarRanges,
                     std::vector<int64_t> intRep );

    bool operator==( const EquivalencePart &key ) const;

    /*int numFixedVars() const {
      return std::count( fixedVars.begin(), fixedVars.end(), true );
    }

    int numUnfixedVars() const {
      return std::count( fixedVars.begin(), fixedVars.end(), false );
    }*/

    std::tuple<size_t, size_t, size_t, size_t> size() const {
      size_t s1 = numBytes(this->boolRep);
      size_t s2 = this->unfixedVarBounds.capacity() * sizeof(decltype(this->unfixedVarBounds)::value_type);
      size_t s3 = this->intRep.capacity() * sizeof(decltype(this->intRep)::value_type);

      return {
        sizeof(*this) + s1 + s2 + s3,
        s1,
        s2,
        s3
      };
    }

    void print() const;

    friend std::hash<EquivalencePart>;
};

template<>
struct std::hash<EquivalencePart> {
  std::size_t operator()( const EquivalencePart &key ) const noexcept {
    return key.hash_;
  }
};

#endif //CHUFFED_EQUIVALENCEPART_H
