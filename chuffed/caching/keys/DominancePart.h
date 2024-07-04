#ifndef CHUFFED_DOMINANCEPART_H
#define CHUFFED_DOMINANCEPART_H

#include <memory>
#include <vector>
#include <unordered_map>

#include "chuffed/caching/keys/constraints/DomConstraintKey.h"
#include "VariableDomain.h"

class DominancePart {
  private:
    std::vector<std::unique_ptr<DomConstraintKey>> constraints;
    std::unordered_map<int, VariableDomain> variables;

  public:
    DominancePart(
      std::vector<std::unique_ptr<DomConstraintKey>> constraints,
      std::unordered_map<int, VariableDomain> variables
    ) : constraints( std::move( constraints ) ), variables( std::move( variables ) ) {}

    bool dominates( const DominancePart &p ) const {
      if ( constraints.size() != p.constraints.size() || p.variables.size() < variables.size() ) {
        //std::cerr << "Unequal length!" << std::endl;
        return false;
      }

      for ( int i = 0; i < constraints.size(); ++i ) {
        if ( constraints[i]->id != p.constraints[i]->id || !constraints[i]->dominates(*p.constraints[i]) ) {
          return false;
        }
      }

      /*
       * The set of variables does not contain a variable if its domain is equal to its original domain (at the start of the search).
       * In order to check the dominance of the variables, each variable that appears in p must be dominated by a variable in this object,
       * which is the case if there is a variable in the set that dominates it or if that variable does not appear
       * in the set of this object (as the original domain is always a superset of any smaller domain). However, this
       * object should not have any variables that do not appear in p, as those do not dominate p.
       */
      int matches = 0;

      for ( const std::pair<const int, VariableDomain> &pair : p.variables) {
        auto it = variables.find( pair.first );

        if ( it != variables.end() ) {
          ++matches;

          if ( !it->second.dominates( pair.second ) ) {
            return false;
          }
        }
      }

      // If the number of variables of p that matched a variable of this object is not equal to the number of variables,
      // it means that this object has a variable that does not appear in p, which is then dominated by p since that variable
      // has its original domain in p.
      return matches == variables.size();
    }

    std::tuple<size_t, size_t, size_t> size() const {
      size_t s = 0;
      for ( auto const &v: variables ) {
        s += sizeof(v.first) + v.second.size();
      }

      size_t constraint_size = 0;
      for ( auto const &c: constraints ) {
        constraint_size += c->size();
      }

      size_t s2 = (this->constraints.capacity() * sizeof(decltype(this->constraints)::value_type)) + constraint_size;

      return {
        sizeof(*this) + s + s2,
        s,
        s2
      };
    }

    void print() const {
      //constraints.print();
      //variables.print();
    }
};

#endif //CHUFFED_DOMINANCEPART_H
