#include "EventCache.h"

#include <iostream>
#include <fstream>
#include <regex>

template <class EqKey, class DomKey>
EventCache<EqKey, DomKey>::EventCache( const std::string &eventFile ) {
  std::string line;
  std::ifstream file(eventFile);
  std::regex delim(",");

  if ( !file.is_open() ) {
    std::cerr << "Cannot open event file " << eventFile << "." << std::endl;
    exit(EXIT_FAILURE);
  }

  while ( getline(file, line) ) {
    std::vector<std::string> list( std::sregex_token_iterator( line.begin(), line.end(), delim, -1), std::sregex_token_iterator() );

    if ( list[1] == "CACHE_HIT" ) {
      int entry = stoi(list[2]);

      this->cacheHits[ stoi(list[0]) ] = entry;

      if ( this->numHits.find(entry) != this->numHits.end() ) {
        this->numHits[entry] = this->numHits[entry] + 1;
      } else {
        this->numHits[entry] = 1;
      }
    }
  }

  file.close();
}

template <class EqKey, class DomKey>
bool EventCache<EqKey, DomKey>::insert( ProjectionKey<EqKey, DomKey> key, int nodeId ) {
  if ( this->numHits.find( key.id ) != this->numHits.end() ) {
    return UnboundedCache<EqKey, DomKey>::insert( std::move( key ), nodeId );
  }

  return false;
}

template <class EqKey, class DomKey>
std::pair<int, bool> EventCache<EqKey, DomKey>::find( const ProjectionKey<EqKey, DomKey> &key, int nodeId ) {
  std::pair<int, bool> pair = UnboundedCache<EqKey, DomKey>::find( key, nodeId );

  if ( pair.second ) {
    // The unordered map does not store the elements in a certain order, which means that iterating over its contents
    // can produce a different hit. Normally, this is fine, but it does not work for the event cache as it relies on
    // the hit occurring for a particular key. Therefore, this cache explicitly keeps track of the identifier of the key
    // that is supposed to produce the hit, allowing the correct one to be updated even when the lookup yields a different
    // key.
    int id = this->cacheHits[ key.id ];

    // Reduce the number of hits by one and remove the entry if the number of hits becomes zero.
    this->numHits[id] = this->numHits[id] - 1;

    if ( this->numHits[id] == 0 ) {
      this->numHits.erase( id );
      this->erase( id );
    }
  }

  return pair;
}
