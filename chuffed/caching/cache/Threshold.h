#ifndef CHUFFED_THRESHOLD_H
#define CHUFFED_THRESHOLD_H

#include <unordered_map>
#include <list>
#include <utility>

#include "chuffed/caching/cache/UnboundedCache.h"

template <class EqKey, class DomKey>
class Threshold: public UnboundedCache<EqKey, DomKey> {
  private:
    typedef int keyId;
    typedef int lastHit;

    int interval;
    int threshold;
    int prevNodeId = 0;

    // TODO: This data is practically the same as what the LRU policy records.
    std::list<std::pair<keyId, lastHit>> elements;
    std::unordered_map<keyId, typename std::list<std::pair<keyId, lastHit>>::iterator> cacheEntryMap;

    void prune(int nodeId);

  public:
    Threshold( int interval, int threshold );

    bool insert( ProjectionKey<EqKey, DomKey> key, int nodeId ) override;

    std::pair<int, bool> find( const ProjectionKey<EqKey, DomKey> &key, int nodeId ) override;
};

#include "Threshold_impl.h"

#endif //CHUFFED_THRESHOLD_H
