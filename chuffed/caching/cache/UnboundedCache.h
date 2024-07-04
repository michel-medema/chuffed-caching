#ifndef CHUFFED_UNBOUNDEDCACHE_H
#define CHUFFED_UNBOUNDEDCACHE_H

#include <cstdio>
#include <utility>

#include "ICache.h"
#include "chuffed/caching/keys/ProjectionKey.h"
#include "chuffed/caching/cache/events/CacheEventStore.h"

template <class EqKey, class DomKey>
class UnboundedCache: public ICache<EqKey, DomKey> {
  protected:
    int cacheHits = 0;
    size_t n = 0;

    size_t maximumKeys = 0;
    size_t maximumSize = 0;

    void erase( int id ) override {
      auto it = this->identifierMap.find(id);

      if ( it != this->identifierMap.end() ) {
        auto entry = this->cache.find( *it->second );

        if ( entry != this->cache.end() ) {
          // Remove the cache entry from the list to which the element is pointing.
          n = n - entry->second.erase( id );

          // If the map associated with the key is now empty, remove the key as well.
          if ( entry->second.empty() ) {
            this->cache.erase( *it->second );
          }
        }

        // Erase the key stored for this identifier.
        this->identifierMap.erase( it );
      }
    }

  public:
    bool insert( ProjectionKey<EqKey, DomKey> key, int nodeId ) override {
      // This statement returns an iterator to the element with the specified key. If such a key does not exist,
      // it is first inserted with an empty list as its value. Otherwise, the iterator points to the existing element.
      auto state = this->cache.emplace( std::move( key.equivalencePart ), std::unordered_map<int, DomKey>() ).first;

      #ifdef LOG_CACHE_EVENTS
      for ( const auto &item: state->second ) {
        if ( item.second.dominates( key.dominancePart ) ) { // If p is dominated, it does not have to be added to the list.

          eventStore.addEvent( CacheEvent(nodeId, CACHE_EVENTS::REJECTED, item.first) );

          return false;
        }
      }
      #endif

      state->second.insert( { key.id, std::move( key.dominancePart ) } );
      ++n;

      // Map the identifier of this key to the equivalence part.
      this->identifierMap.insert( { key.id, &state->first} );

      if ( n > maximumSize ) {
        maximumSize = n;
        maximumKeys = this->numKeys();
      }

#ifdef LOG_CACHE_EVENTS
      eventStore.addEvent( CacheEvent( nodeId, CACHE_EVENTS::INSERTED, key.id, key.decisionLevel ) );
#endif

      return true;
    }

    std::pair<int, bool> find( const ProjectionKey<EqKey, DomKey> &key, int nodeId ) override {
      auto entry = this->cache.find( key.equivalencePart );

      if ( entry != this->cache.end() ) {
        for ( const auto &item: entry->second ) {
          if ( item.second.dominates( key.dominancePart ) ) {
#ifdef LOG_CACHE_EVENTS
            eventStore.addEvent(CacheEvent(nodeId, CACHE_EVENTS::CACHE_HIT, item.first));
#endif

            ++cacheHits;
            return {item.first, true};
          }
        }
      }

      return {-1, false};
    }

    int hits() const override { return cacheHits; }

    /*size_t averageKeySize() const override {
      size_t eqBoolrep = 0;
      size_t eqUnfixedVarSize = 0;
      size_t eqIntRep = 0;
      size_t domVarSize = 0;
      size_t domConsSize = 0;

      size_t maxKeySize = 0;

      size_t size = 0;

      for ( const auto &entry : this->cache ) {
        std::tuple<size_t, size_t, size_t, size_t> eqSize = entry.first.size();

        size += std::get<0>(eqSize);// + sizeof(typename decltype(this->cache)::mapped_type);

        if ( std::get<0>(eqSize) > maxKeySize ) {
          maxKeySize = std::get<0>(eqSize);
        }

        eqBoolrep += std::get<1>( eqSize);
        eqUnfixedVarSize += std::get<2>(eqSize);
        eqIntRep += + std::get<3>( eqSize);

        for ( const auto &item: entry.second ) {
          std::tuple<size_t, size_t, size_t> domSize = item.second.size();

          size += + std::get<0>(domSize); //+ sizeof(item.first)

          domVarSize += std::get<1>(domSize);
          domConsSize += + std::get<2>(domSize);
        }
      }

      std::cout << "Total size: " << size << std::endl;
      std::cout << "Total eqBoolRep size: " << eqBoolrep << std::endl;
      std::cout << "Total eqUnfixedVar size: " << eqUnfixedVarSize << std::endl;
      std::cout << "Total eqIntRep size: " << eqIntRep << std::endl;
      std::cout << "Total domVar size: " << domVarSize << std::endl;
      std::cout << "Total domCons size: " << domConsSize << std::endl;

      //std::cout << "Max size: " << maxKeySize << std::endl;

      //std::cout << "ID map size: " << sizeof(this->identifierMap) + this->identifierMap.size() * (sizeof(typename decltype(this->identifierMap)::key_type) + sizeof(typename decltype(this->identifierMap)::mapped_type)) << std::endl;

      return size / n;
    }*/

    bool empty() const override { return n == 0; }

    size_t size() const override { return n; }

    size_t numKeys() const override { return this->cache.size(); }

    size_t maxSize() const override { return this->maximumSize; }

    size_t maxKeys() const override { return this->maximumKeys; }
};

#endif