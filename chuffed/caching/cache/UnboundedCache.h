#ifndef CHUFFED_UNBOUNDEDCACHE_H
#define CHUFFED_UNBOUNDEDCACHE_H

#include <cstdio>
#include <utility>
#include <unordered_map>

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

			/*
			 * Technically, it is possible for a new key to dominate a key already in the cache or
			 * to be dominated by a key in the cache. In the first case, the dominated keys could
			 * be removed; in the latter case, the new key can be rejected. However, some experimentation
			 * shows that these situations hardly ever, if ever, occur, making it not worth the
			 * additional computational complexity of performing such checks. It is only done when events
			 * are logged so that its occurrence can be analysed.
			 */
			for ( auto it = state->second.begin(); it != state->second.end(); ) {
				if ( it->second.dominates( key.dominancePart ) ) { // If p is dominated, it does not have to be added to the cache.
					#ifdef LOG_CACHE_EVENTS
					eventStore.addEvent( CacheEvent(nodeId, CACHE_EVENTS::REJECTED, it->first) );
					#endif

					return false;
				}

				if ( key.dominancePart.dominates( it->second ) ) { // If an existing entry is dominated by the new key, it can be removed.
					#ifdef LOG_CACHE_EVENTS
					eventStore.addEvent( CacheEvent(nodeId, CACHE_EVENTS::SUPERSEDED, it->first) );
					#endif

					// Remove the dominated entry from the cache. For performance reasons and to ensure that
					// the iterator points to the correct element in the unordered map after erasure, this
					// operation cannot use the erase method of the UnboundedCache.
					this->identifierMap.erase( it->first );
					it = state->second.erase( it );
					--n;
				} else {
					++it;
				}
			}

      state->second.insert( { key.id, std::move( key.dominancePart ) } );
      ++n;

      // Map the identifier of this key to the equivalence part.
      this->identifierMap.insert( { key.id, &state->first} );

      if ( n > maximumSize ) {
        maximumSize = n;
        maximumKeys = this->numKeys();
      }

#ifdef LOG_CACHE_EVENTS
      eventStore.addEvent( CacheEvent( nodeId, CACHE_EVENTS::INSERTED, key.id, key.decisionLevel, key.fixedVars, key.unfixedVars ) );
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

    bool empty() const override { return n == 0; }

    size_t size() const override { return n; }

    size_t numKeys() const override { return this->cache.size(); }

    size_t maxSize() const override { return this->maximumSize; }

    size_t maxKeys() const override { return this->maximumKeys; }
};

#endif