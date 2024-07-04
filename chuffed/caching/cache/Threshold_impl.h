#include "Threshold.h"

template <class EqKey, class DomKey>
Threshold<EqKey, DomKey>::Threshold( int interval, int threshold ) {
  if ( interval < 1 || threshold < 1 ) {
    throw std::invalid_argument("The prune interval and threshold must be larger than zero.");
  }

  this->interval = interval;
  this->threshold = threshold;
}

template <class EqKey, class DomKey>
bool Threshold<EqKey, DomKey>::insert( ProjectionKey<EqKey, DomKey> key, int nodeId ) {
  this->prune( nodeId );

  int id = key.id;
  bool inserted = UnboundedCache<EqKey, DomKey>::insert( std::move( key), nodeId );

  if ( inserted ) {
    this->elements.emplace_back( id, nodeId );
    this->cacheEntryMap.insert_or_assign( id, --this->elements.end() );
  }

  return inserted;
}

template <class EqKey, class DomKey>
std::pair<int, bool> Threshold<EqKey, DomKey>::find( const ProjectionKey<EqKey, DomKey> &key, int nodeId ) {
  this->prune( nodeId );

  std::pair<int, bool> result = UnboundedCache<EqKey, DomKey>::find( key, nodeId );

  if ( result.second ) {
    auto it = this->cacheEntryMap[result.first];

    this->elements.emplace_back( result.first, nodeId );
    this->elements.erase( it );
    this->cacheEntryMap.insert_or_assign( result.first, --this->elements.end() );
  }

  return result;
}

template <class EqKey, class DomKey>
void Threshold<EqKey, DomKey>::prune( int nodeId ) {
  if ( nodeId - prevNodeId >= interval ) {
    for ( auto it = this->elements.begin(); it != this->elements.end(); ) {
      // The elements are sorted based on recency. As soon as an element is encountered that does not exceed
      // the threshold, the entire pruning process can be stopped, since the remaining elements will also not
      // exceed the threshold.
      if ( nodeId - it->second < threshold ) {
        break;
      }

      int id = it->first;

      // Remove the element from the cache.
      UnboundedCache<EqKey, DomKey>::erase( id );

      // Remove all the data of the element recorded by the strategy.
      auto it1 = cacheEntryMap.find( id );

      if ( it1 != cacheEntryMap.end() ) {
        it = this->elements.erase( it1->second );
        this->cacheEntryMap.erase( it1 );
      } else {
        ++it;
      }
    }

    prevNodeId = nodeId;
  }
}