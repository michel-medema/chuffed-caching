#ifndef CHUFFED_BOUNDEDCACHE_H
#define CHUFFED_BOUNDEDCACHE_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <cmath>

#include "chuffed/caching/cache/UnboundedCache.h"
#include "chuffed/caching/cache/policies/EvictionPolicy.h"

template <class EqKey, class DomKey>
class BoundedCache: public UnboundedCache<EqKey, DomKey> {
  protected:
    int cacheSize;
    int pruneSize;

    std::unique_ptr<EvictionPolicy> policy;

    void erase( int id ) override {
      UnboundedCache<EqKey, DomKey>::erase( id );
      policy->erased( id );
    }

  public:
    BoundedCache( int cacheSize, int pruneSize, std::unique_ptr<EvictionPolicy> policy ): policy( std::move(policy) ) {
      if ( cacheSize < 1 || pruneSize < 1 ) {
        throw std::invalid_argument("The cache size and prune size must be larger than zero.");
      }

      this->cacheSize = cacheSize;
      this->pruneSize = pruneSize;
    };

    bool insert( ProjectionKey<EqKey, DomKey> key, int nodeId ) override {
      if ( this->size() >= this->cacheSize ) {
        int n = std::min(this->pruneSize, (int) this->size());

        while ( this->size() > (this->cacheSize - n) ) {
          int candidate = policy->getCandidate();
          this->erase( candidate );
        }
      }

      int id = key.id;
      bool inserted = UnboundedCache<EqKey, DomKey>::insert( std::move( key), nodeId );

      if ( inserted ) {
        policy->inserted(id);
      }

      return inserted;
    }

    std::pair<int, bool> find( const ProjectionKey<EqKey, DomKey> &key, int nodeId ) override {
      std::pair<int, bool> result = UnboundedCache<EqKey, DomKey>::find( key, nodeId );

      if ( result.second ) {
        policy->cacheHit(result.first);
      }

      return result;
    }
};

#endif //CHUFFED_BOUNDEDCACHE_H
