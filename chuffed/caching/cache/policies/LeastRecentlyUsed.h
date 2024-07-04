#ifndef CHUFFED_LEASTRECENTLYUSED_H
#define CHUFFED_LEASTRECENTLYUSED_H

#include <list>
#include <unordered_map>

#include "EvictionPolicy.h"

class LeastRecentlyUsed: public EvictionPolicy {
  private:
    std::list<int> elements;
    std::unordered_map<int, typename std::list<int>::iterator> cacheEntryMap;

  public:
    void inserted( int id ) override {
      auto it = cacheEntryMap.find( id );

      if ( it != cacheEntryMap.end() ) {
        // If the element already exists, erase it so that it can be added again at the back of the list.
        elements.erase( it->second );
      }

      this->elements.emplace_back( id );
      this->cacheEntryMap.insert_or_assign( id, --this->elements.end() );
    }

    void cacheHit( int id ) override {
      // TODO: This ID may not exist.
      auto it = this->cacheEntryMap[id];

      this->elements.emplace_back( *it );
      this->elements.erase( it );
      this->cacheEntryMap.insert_or_assign( id, --this->elements.end() );
    }

    void erased( int id ) override {
      auto it = cacheEntryMap.find( id );

      if ( it != cacheEntryMap.end() ) {
        this->elements.erase( it->second );
        this->cacheEntryMap.erase( it );
      }
    }

    int getCandidate() override {
      if ( elements.empty() ) {
        return -1;
      }

      return elements.front();
    }
};


#endif //CHUFFED_LEASTRECENTLYUSED_H
