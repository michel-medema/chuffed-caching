#ifndef CHUFFED_CACHEEVENTSTORE_H
#define CHUFFED_CACHEEVENTSTORE_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "chuffed/core/options.h"

#include "CacheEvent.h"


static const std::string cacheEvents[] = {"CACHE_HIT", "INSERTED", "REJECTED", "SUPERSEDED", "RESTART", "FINISHED"};

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

            file << "iteration,event,entry,decisionLevel,fixedVars,unfixedVars\n";

            for ( const CacheEvent &event: events ) {
                file << event.iteration << "," << cacheEvents[event.type] << "," << event.entry << ","
                     << event.decisionLevel << "," << event.fixedVars << "," << event.unfixedVars << '\n';
            }

            file.close();
        }
};

extern CacheEventStore eventStore;

#endif //CHUFFED_CACHEEVENTSTORE_H
