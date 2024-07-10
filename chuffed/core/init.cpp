#include "chuffed/core/engine.h"
#include "chuffed/core/options.h"
#include "chuffed/core/sat.h"
#include "chuffed/globals/globals.h"
#include "chuffed/ldsb/ldsb.h"
#include "chuffed/mip/mip.h"

#include "chuffed/caching/cache/BoundedCache.h"
#include "chuffed/caching/cache/Threshold.h"
#include "chuffed/caching/cache/EventCache.h"
#include "chuffed/caching/cache/UnboundedCache.h"

#include "chuffed/caching/cache/policies/RandomReplacement.h"
#include "chuffed/caching/cache/policies/LeastRecentlyUsed.h"
#include "chuffed/caching/cache/policies/FIFO.h"

#include <iostream>
#include <memory>

void process_ircs();

void Engine::init() {
	// Get the propagators ready
	process_ircs();

	// Get well-founded propagators ready
	wf_init();

	// Get the vars ready
	for (int i = 0; i < vars.size(); i++) {
		IntVar* v = vars[i];
		if (v->pinfo.size() == 0) {
			v->in_queue = true;
		} else {
			v->pushInQueue();
		}
	}

	if (so.lazy) {
		for (int i = 0; i < vars.size(); i++) {
			if (vars[i]->getMax() - vars[i]->getMin() <= so.eager_limit) {
				vars[i]->specialiseToEL();
			} else {
				if (so.verbosity >= 2) {
					std::cerr << "using lazy literal\n";
				}
				vars[i]->specialiseToLL();
			}
		}
	} else {
		for (int i = 0; i < vars.size(); i++) {
			vars[i]->initVals(true);
		}
	}

	// Get MIP propagator ready

	if (so.mip) {
		mip->init();
	}

	// Get SAT propagator ready

	sat.init();

	// Set lits allowed to be in learnt clauses
	problem->restrict_learnable();

	// Get LDSB ready

	if (so.ldsb) {
		ldsb.init();
	}

	// Do MIP presolve

	if (so.mip) {
		mip->presolve();
	}

	if ( so.caching ) {
		if ( so.verbosity >= 1 ) {
			std::cerr << "Caching enabled.\n";
		}

		switch ( so.cacheStrategy ) {
			case 1:
				if ( so.verbosity >= 1 ) {
					std::cerr << "Using random replacement strategy.\n";
				}
				engine.cache = std::make_unique<BoundedCache<EquivalencePart, DominancePart>>( so.cacheSize, so.pruneSize, std::make_unique<RandomReplacement>() );
				break;
			case 2:
				if ( so.verbosity >= 1 ) {
					std::cerr << "Using least recently used strategy.\n";
				}
				engine.cache = std::make_unique<BoundedCache<EquivalencePart, DominancePart>>( so.cacheSize, so.pruneSize, std::make_unique<LeastRecentlyUsed>() );
				break;
			case 3:
				if ( so.verbosity >= 1 ) {
					std::cerr << "Using FIFO strategy.\n";
				}
				engine.cache = std::make_unique<BoundedCache<EquivalencePart, DominancePart>>( so.cacheSize, so.pruneSize, std::make_unique<FIFO>() );
				break;
			case 4:
				if ( so.verbosity >= 1 ) {
					std::cerr << "Using threshold-based strategy.\n";
				}
				engine.cache = std::make_unique<Threshold<EquivalencePart, DominancePart>>( so.pruneInterval, so.pruneThreshold );
				break;
			case 5:
				if ( so.verbosity >= 1 ) {
					std::cerr << "Using cache with event file " << so.cacheEvents << ".'\n'";
				}
				engine.cache = std::make_unique<EventCache<EquivalencePart, DominancePart>>( so.cacheEvents );
				break;
			default:
				if ( so.verbosity >= 1 ) {
					std::cerr << "Using unbounded cache.\n";
				}
				engine.cache = std::make_unique<UnboundedCache<EquivalencePart, DominancePart>>( );
				break;
		}
	}

	// Ready

	finished_init = true;
}
