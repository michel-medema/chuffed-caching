#ifndef CHUFFED_CACHINGCONSTRAINT_H
#define CHUFFED_CACHINGCONSTRAINT_H

#include <optional>
#include <functional>

#include "chuffed/core/propagator.h"

class CachingConstraint : public Propagator {
    protected:
        int n;
        bool keyIsTrue = false;
        Tchar fixed;

        virtual std::vector<int> scope() const {
            return std::vector<int>{};
        };

    public:
        explicit CachingConstraint(): CachingConstraint(0) {}

        explicit CachingConstraint(int n): n(n), fixed(0), Propagator() {
            setCacheSupport();
        }

        bool includesObjectiveVar() const {
            if ( engine.opt_var == nullptr ) {
                return false;
            }

            for ( int v: this->scope() ) {
                if ( v == engine.opt_var->var_id ) {
                    return true;
                }
            }

            return false;
        }

        virtual void initialise() {
          n = n - fixed;
          fixed = 0;
        }

        bool isSatisfied() const {
            return keyIsTrue || satisfied || fixed >= n;
        }

        bool isInactive() const {
            return fixed == 0;
        }

        template<int I>
        static std::optional<int64_t> val( const IntView<I> &v ) {
            if ( v.isFixed() ) {
                return v.getVal();
            } else {
                return {};
            }
        }
};

#endif //CHUFFED_CACHINGCONSTRAINT_H