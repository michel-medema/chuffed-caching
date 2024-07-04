#ifndef CHUFFED_ICACHE_H
#define CHUFFED_ICACHE_H

#include <cstdio>
#include <unordered_map>
#include <utility>
#include <iostream>

#include "chuffed/caching/keys/ProjectionKey.h"

template <class EqKey, class DomKey>
class ICache {
  protected:
    std::unordered_map<EqKey, std::unordered_map<int, DomKey>> cache;
    std::unordered_map<int, const EqKey*> identifierMap;

    virtual void erase( int id ) = 0;

  public:
    virtual bool insert( ProjectionKey<EqKey, DomKey> key, int nodeId ) = 0;

    virtual std::pair<int, bool> find( const ProjectionKey<EqKey, DomKey> &key, int nodeId ) = 0;

    virtual int hits() const = 0;

    //virtual size_t averageKeySize() const = 0;

    virtual bool empty() const = 0;

    virtual size_t size() const = 0;

    virtual size_t numKeys() const = 0;

    virtual size_t maxSize() const = 0;

    virtual size_t maxKeys() const = 0;

    virtual ~ICache() = default;

    // NOTE: This function is defined in this way rather than adding it to the class because the unit tests use
    // primitive types for the equivalence part and these do not have a size function.
    static size_t avgKeySize( const ICache<EqKey, DomKey>& cache ) {
			if ( cache.size() < 1 ) { // Prevent division by zero when the cache is empty.
				return 0;
			}

      size_t eqBoolrep = 0;
      size_t eqUnfixedVarSize = 0;
      size_t eqIntRep = 0;
      size_t domVarSize = 0;
      size_t domConsSize = 0;

      size_t maxKeySize = 0;

      size_t size = 0;

      for ( const auto &entry : cache.cache ) {
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

      return size / cache.size();
    }
};

#endif //CHUFFED_ICACHE_H
