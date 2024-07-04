#ifndef CHUFFED_STATEBUILDER_H
#define CHUFFED_STATEBUILDER_H

#include <vector>
#include <set>

#include "chuffed/support/vec.h"

#include "chuffed/caching/keys/EquivalencePart.h"
#include "chuffed/caching/keys/DominancePart.h"

class IntVar;

class DominanceConstraint;

class EquivalenceConstraint;

class BoolView;

class Boolean;

using std::vector, std::pair;

class StateBuilder {
  private:
    vector<pair<IntVar*, VariableDomain>> variables;
    vector<BoolView> booleans;

    const IntVar *objectiveVariable;

    vector<DominanceConstraint *> dominanceConstraints;
    vector<EquivalenceConstraint *> equivalenceConstraints;

    vector<Boolean *> booleanConstraints;

    static bool domainChanged( const IntVar *var, const VariableDomain &dom );

  public:
    StateBuilder(
      vec<IntVar *> variables,
      const std::set<BoolView> &booleans,
      const IntVar *objectiveVariable,
      const vector<Boolean *> &booleanConstraints,
      const vector<EquivalenceConstraint *> &equivalenceConstraints,
      const vector<DominanceConstraint *> &dominanceConstraints
    );

    EquivalencePart getEquivalencePart() const;

    DominancePart getDominancePart() const;
};


#endif //CHUFFED_STATEBUILDER_H
