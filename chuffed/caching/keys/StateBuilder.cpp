#include "StateBuilder.h"

#include <unordered_map>

#include "chuffed/vars/int-var.h"

#include "chuffed/caching/keys/EquivalencePart.h"
#include "chuffed/caching/keys/DominancePart.h"
#include "chuffed/caching/propagators/EquivalenceConstraint.h"
#include "chuffed/caching/propagators/DominanceConstraint.h"
#include "chuffed/caching/propagators/Boolean.h"

StateBuilder::StateBuilder(
  vec<IntVar *> variables,
  const std::set<BoolView> &booleans,
  const IntVar *objectiveVariable,
  const vector<Boolean *> &booleanConstraints,
  const vector<EquivalenceConstraint *> &equivalenceConstraints,
  const vector<DominanceConstraint *> &dominanceConstraints
) : objectiveVariable( objectiveVariable ) {
  this->variables.reserve( variables.size() );
  this->booleans.reserve( booleans.size() );
  this->booleanConstraints.reserve( booleanConstraints.size() );
  this->equivalenceConstraints.reserve( equivalenceConstraints.size() );
  this->dominanceConstraints.reserve( dominanceConstraints.size() );

  for ( int i = 0; i < variables.size(); i++ ) {
    IntVar *v = variables[i];

    if ( !v->isFixed() && v != objectiveVariable ) this->variables.emplace_back( v, VariableDomain::getDomain( v ) );
  }

  for ( const BoolView &b: booleans ) {
    if ( !b.isFixed() ) this->booleans.push_back( b );
  }

  for ( Boolean *b: booleanConstraints ) {
    b->initialise();
    if ( !b->includesObjectiveVar() && !b->isSatisfied() ) this->booleanConstraints.push_back( b );
  }

  for ( EquivalenceConstraint *c: equivalenceConstraints ) {
    c->initialise();
    if ( !c->includesObjectiveVar() && !c->isSatisfied() ) this->equivalenceConstraints.push_back( c );
  }

  for ( DominanceConstraint *c: dominanceConstraints ) {
    c->initialise();
    if ( !c->includesObjectiveVar() && !c->isSatisfied() ) this->dominanceConstraints.push_back( c );
  }

  this->variables.shrink_to_fit();
  this->booleans.shrink_to_fit();
  this->booleanConstraints.shrink_to_fit();
  this->equivalenceConstraints.shrink_to_fit();
  this->dominanceConstraints.shrink_to_fit();
}

bool StateBuilder::domainChanged( const IntVar *var, const VariableDomain &dom ) {
  // In general, the size of the domain could change by swapping two values, which would still imply a change,
  // but since the variables are stored at the beginning of the search after the initial propagation, the
  // domains of the variables should only become smaller by removing existing values from it.
  return var->getMin() != dom.min || var->size() != dom.domainSize;
}

EquivalencePart StateBuilder::getEquivalencePart() const {
  std::vector<std::pair<int, int>> unfixedVarRanges;
  std::vector<int64_t> intRep;
  std::vector<bool> boolRep;

  boolRep.reserve( 2 * variables.size() + booleans.size() + booleanConstraints.size() );
  intRep.reserve( equivalenceConstraints.size() );
  if ( !so.varDominanceCheck ) unfixedVarRanges.reserve( variables.size() );

  for ( const BoolView &b: booleans ) {
    boolRep.push_back( b.isFixed() );
  }

  int i = 0;

  for ( const auto& [var, dom]: variables ) {
    // The representation encodes which variables are fixed and which variables have changed since the start of the
    // search in order to capture whose domains are included. Combined with the min and max values, this encoding ensures
    // that it is not possible for two equivalence parts in which the individual variables are different but their combination
    // is somehow identical to be considered equal (e.g. one key may have two variables with 1111 + 111 and another with
    // 111 + 1111, which would be considered identical without additional information).
    boolRep.push_back( var->isFixed() );
    boolRep.push_back( StateBuilder::domainChanged( var, dom ) );

    if ( !so.varDominanceCheck && !var->isFixed() && StateBuilder::domainChanged( var, dom ) ) {
    //if ( !so.varDominanceCheck && !var->isFixed() && !VariableDomain::originalDomain(var) ) {
      for ( int val = var->getMin(); val <= var->getMax(); ++val ) {
        boolRep.emplace_back( var->indomain( val ) );
      }

      /*int maxSize = (var->getMax() - var->getMin()) + 1;

      if ( VariableDomain::completeDomain( var ) ) { // All values between min and max are in the domain.
        boolRep.insert( boolRep.end(), maxSize, true );
      } else {
        boolRep.insert( boolRep.end(), maxSize, false );

        for ( auto val: *var ) {
          boolRep[i + (val - var->getMin())] = true;
        }

        i = i + maxSize;
      }*/

      // The minimum and maximum values are both stored to ensure that the combination of two different domains is never
      // considered equal (e.g. 11101 + 11 == 11 + 10111).
      unfixedVarRanges.emplace_back( var->getMin(), var->getMax() );
    }
  }

  for ( const Boolean *b: booleanConstraints ) {
    boolRep.push_back( b->satisfied );
  }

  for ( const EquivalenceConstraint *c: equivalenceConstraints ) {
    if ( !c->isInactive() && !c->isSatisfied() ) {
      intRep.push_back( c->prop_id );
      c->projectionKey( intRep, boolRep );
    }
  }

  unfixedVarRanges.shrink_to_fit();
  intRep.shrink_to_fit();
  boolRep.shrink_to_fit();

  return { std::move( boolRep ), std::move( unfixedVarRanges ), std::move( intRep ) };
}

DominancePart StateBuilder::getDominancePart() const {
  std::unordered_map<int, VariableDomain> domains;
  std::vector<std::unique_ptr<DomConstraintKey>> constraints;

  constraints.reserve( this->dominanceConstraints.size() );

  if ( so.varDominanceCheck ) {
    for ( const auto& [v, dom]: this->variables ) {
      if ( !v->isFixed() && StateBuilder::domainChanged( v, dom ) ) {
        domains.emplace( v->var_id, VariableDomain::getDomain( v ) );
      }
    }
  }

  if ( objectiveVariable != nullptr ) {
    domains.emplace( objectiveVariable->var_id, VariableDomain::getDomain( objectiveVariable ) );
  }

  for ( const DominanceConstraint *c: dominanceConstraints ) {
    if ( !c->isInactive() && !c->isSatisfied() ) {
      constraints.push_back( c->projectionKey() );
    }
  }

  constraints.shrink_to_fit();

  return { std::move( constraints ), std::move( domains ) };
}
