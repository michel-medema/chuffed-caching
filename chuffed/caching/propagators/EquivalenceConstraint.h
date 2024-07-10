#ifndef CHUFFED_EQUIVALENCECONSTRAINT_H
#define CHUFFED_EQUIVALENCECONSTRAINT_H

#include <vector>

#include "CachingConstraint.h"

class EquivalenceConstraint : public CachingConstraint {
    protected:
        //virtual std::vector<int> scope() const override = 0;

    public:
        explicit EquivalenceConstraint(int n, bool add = true): CachingConstraint(n) {
          if ( add ) {
            engine.equivalenceConstraints.push_back( this );
          }
        }

        //virtual std::unique_ptr<EqConstraintKey> projectionKey() const = 0;
        virtual void projectionKey( std::vector<int64_t>& ints, std::vector<bool>& bools ) const = 0;
};

#endif //CHUFFED_EQUIVALENCECONSTRAINT_H
