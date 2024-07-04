#ifndef CHUFFED_RANDOMREPLACEMENT_H
#define CHUFFED_RANDOMREPLACEMENT_H

#include <vector>
#include <unordered_map>
#include <random>

#include "EvictionPolicy.h"

class RandomReplacement: public EvictionPolicy {
  private:
    std::vector<int> elements;
    std::unordered_map<int, int> cacheEntryMap;

    std::random_device r;
    std::ranlux48_base rng = std::ranlux48_base( r() );

  public:
    void inserted( int id ) override {
      if ( cacheEntryMap.find( id ) == cacheEntryMap.end() ) {
        elements.emplace_back( id );
        cacheEntryMap.insert( {id, elements.size() - 1} );
      }
    }

    void cacheHit( int id ) override {
      /* No updates needed when cache hits occur. */
    }

    void erased( int id ) override {
      auto it = cacheEntryMap.find( id );

      if ( it != cacheEntryMap.end() ) {
        // To keep the vector contiguous, copy the last element to the position of the element that should be removed and
        // remove the last element.
        this->elements[it->second] = this->elements.back();
        cacheEntryMap[this->elements.back()] = it->second;
        this->elements.pop_back();

        this->cacheEntryMap.erase( it );
      }
    }

    int getCandidate() override {
      if ( elements.empty() ) {
        return -1;
      }

      std::uniform_int_distribution<size_t> distribution( 0, elements.size() - 1 );
      size_t i = distribution( rng );

      return elements[i];
    }
};

#endif //CHUFFED_RANDOMREPLACEMENT_H
