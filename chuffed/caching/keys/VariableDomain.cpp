#include "VariableDomain.h"

#include <utility>

#include "chuffed/vars/int-var.h"

VariableDomain::VariableDomain( int id, int min, int domainSize, std::vector<bool> values ) : id( id ),
                                                                              min( min ),
                                                                              domainSize( domainSize ),
                                                                              values( std::move( values ) ) {}

bool VariableDomain::operator==( const VariableDomain &v ) const {
  return v.id == id && v.min == min && v.values == values;
}

bool VariableDomain::dominates(
  const VariableDomain &v ) const { // Returns true whenever this domain is a superset of the domain of v.
  if ( v.min < min || (v.min + v.values.size() - 1) > (min + values.size() - 1) ) {
    return false;
  }

  for ( int i = 0; i < v.values.size(); ++i ) {
    if ( v.values[i] ) { // If the value is in the domain of v, it must also be in the domain of this variable.
      // The position of the value v.min + i in the domain of this variable.
      int j = (v.min + i) - min;

      if ( j >= values.size() || !values[j] ) { // This domain does not contain the value.
        return false;
      }
    }
  }

  return true;
}

bool VariableDomain::completeDomain( const IntVar *var ) {
  return var->size() == (var->getMax() - var->getMin()) + 1;
}

bool VariableDomain::originalDomain( const IntVar *var ) {
  return var->getMin0() == var->getMin() && var->getMax0() == var->getMax() && completeDomain( var );
}

VariableDomain VariableDomain::getDomain( const IntVar *var ) {
  int maxSize = (var->getMax() - var->getMin()) + 1;

  if ( VariableDomain::completeDomain( var ) ) { // All values between min and max are in the domain.
    return {var->var_id, var->getMin(), maxSize, std::vector<bool>( maxSize, true )};
  } else {
    int domainSize = 0;
    std::vector<bool> values = std::vector<bool>( maxSize, false );

    for ( auto val: *var ) {
      values[val - var->getMin()] = true;
      ++domainSize;
    }

    return {var->var_id, var->getMin(), domainSize, values};
  }
}
