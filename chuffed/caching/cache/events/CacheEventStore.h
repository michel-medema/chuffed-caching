#ifndef CHUFFED_CACHEEVENTSTORE_H
#define CHUFFED_CACHEEVENTSTORE_H

#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "chuffed/core/options.h"

#include "CacheEvent.h"


static const std::string cacheEvents[] = {"CACHE_HIT", "INSERTED", "SUPERSEDED", "REMOVED", "REJECTED", "RESTART", "FINISHED"};

class CacheEventStore {
    private:
        std::vector<CacheEvent> events;

    public:
        void addEvent( const CacheEvent &event ) {
            events.push_back( event );
        }

        void writeToFile() const {
            std::ofstream file;

            file.open( so.eventFile, std::ofstream::out );

            file << "iteration,event,entry,decisionLevel,fixedVars,unfixedVars" << std::endl;

            for ( const CacheEvent &event: events ) {
                file << event.iteration << "," << cacheEvents[event.type] << "," << event.entry << ","
                     << event.decisionLevel << std::endl;
            }

            file.close();
        }
};

extern CacheEventStore eventStore;

#endif //CHUFFED_CACHEEVENTSTORE_H
