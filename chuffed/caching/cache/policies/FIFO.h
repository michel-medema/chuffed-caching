#ifndef CHUFFED_FIFO_H
#define CHUFFED_FIFO_H

#include <list>
#include <unordered_map>

#include "EvictionPolicy.h"

class FIFO: public EvictionPolicy {
  private:
    std::list<int> elements;
    std::unordered_map<int, typename std::list<int>::iterator> cacheEntryMap;

  public:
    void inserted( int id ) override {
      auto it = cacheEntryMap.find( id );

      if ( it != cacheEntryMap.end() ) {
        // If the element already exists, erase it so that it can be added again at the back of the list.
        this->elements.erase( it->second );
      }

      this->elements.emplace_back( id );
      this->cacheEntryMap.insert_or_assign( id, --this->elements.end() );
    }

    void cacheHit( int id ) override {
      /* The entries do not have to be updated when a cache hit occurs. */
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

#endif //CHUFFED_FIFO_H
