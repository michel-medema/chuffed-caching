#ifndef CHUFFED_CACHEEVENT_H
#define CHUFFED_CACHEEVENT_H

#include "CacheEventStore.h"

enum CACHE_EVENTS {CACHE_HIT, INSERTED, REJECTED, RESTART, FINISHED};

class CacheEvent {
    public:
        const int iteration;
        const CACHE_EVENTS type;
        const int entry;
        const int decisionLevel;
        const int fixedVars;
        const int unfixedVars;

        CacheEvent( int iteration, CACHE_EVENTS type, int entry ) : iteration( iteration ), type( type ),
                                                                    entry( entry ), decisionLevel( 0 ), fixedVars( 0 ), unfixedVars( 0 ) {}

        CacheEvent( int iteration, CACHE_EVENTS type, int entry, int decisionLevel, int fixedVars, int unfixedVars ) :
            iteration( iteration ), type( type ), entry( entry ), decisionLevel( decisionLevel ), fixedVars(fixedVars), unfixedVars(unfixedVars) {}
};

#endif //CHUFFED_CACHEEVENT_H
