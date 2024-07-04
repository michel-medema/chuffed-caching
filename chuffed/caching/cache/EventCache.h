#ifndef CHUFFED_EVENTCACHE_H
#define CHUFFED_EVENTCACHE_H

#include <unordered_map>
#include <string>

#include "chuffed/caching/cache/UnboundedCache.h"

template <class EqKey, class DomKey>
class EventCache: public UnboundedCache<EqKey, DomKey> {
  private:
    std::unordered_map<int, int> numHits;
    std::unordered_map<int, int> cacheHits;

  public:
    explicit EventCache( const std::string &eventFile );

    bool insert( ProjectionKey<EqKey, DomKey> key, int nodeId ) override;

    std::pair<int, bool> find( const ProjectionKey<EqKey, DomKey> &key, int nodeId ) override;
};

#include "EventCache_impl.h"

#endif //CHUFFED_EVENTCACHE_H
