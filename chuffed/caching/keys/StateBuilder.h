#ifndef CHUFFED_STATEBUILDER_H
#define CHUFFED_STATEBUILDER_H

#include <vector>
#include <set>
#include <utility>

#include "chuffed/support/vec.h"

#include "chuffed/caching/keys/EquivalencePart.h"
#include "chuffed/caching/keys/DominancePart.h"
#include "chuffed/caching/keys/VariableDomain.h"

class IntVar;
class DominanceConstraint;
class EquivalenceConstraint;
class BoolView;

class StateBuilder {
  private:
		std::vector<std::pair<IntVar*, VariableDomain>> variables;
		std::vector<BoolView> booleans;

    const IntVar *objectiveVariable;

		std::vector<DominanceConstraint *> dominanceConstraints;
		std::vector<EquivalenceConstraint *> equivalenceConstraints;

    static bool domainChanged( const IntVar *var, const VariableDomain &dom );

  public:
    StateBuilder(
      vec<IntVar *> variables,
      const std::set<BoolView> &booleans,
      const IntVar *objectiveVariable,
      const std::vector<EquivalenceConstraint *> &equivalenceConstraints,
      const std::vector<DominanceConstraint *> &dominanceConstraints
    );

    EquivalencePart getEquivalencePart() const;

    DominancePart getDominancePart() const;
};


#endif //CHUFFED_STATEBUILDER_H
