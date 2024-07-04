#ifndef CHUFFED_DOMINANCECONSTRAINT_H
#define CHUFFED_DOMINANCECONSTRAINT_H

#include <memory>

#include "CachingConstraint.h"

class DominanceConstraint : public CachingConstraint {
    protected:
        virtual std::vector<int> scope() const override = 0;

    public:
        explicit DominanceConstraint(int n): CachingConstraint(n) {
            engine.dominanceConstraints.push_back( this );
        }

        virtual std::unique_ptr<DomConstraintKey> projectionKey() const = 0;
};

#endif //CHUFFED_DOMINANCECONSTRAINT_H
