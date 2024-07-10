#include <utility>
#include <iostream>

#include "EquivalencePart.h"

EquivalencePart::EquivalencePart(
  std::vector<bool> boolRep,
  std::vector<std::pair<int, int>> unfixedVarRanges,
  std::vector<int64_t> intRep
) : boolRep( std::move( boolRep ) ),
			unfixedVarBounds( std::move( unfixedVarRanges ) ), intRep( std::move( intRep ) ), hash_( this->hashValue() ) {}

bool EquivalencePart::operator==( const EquivalencePart &key ) const {
  return boolRep == key.boolRep && unfixedVarBounds == key.unfixedVarBounds && intRep == key.intRep;
}

void EquivalencePart::print() const {
  /*std::cout << "Fixed: ";
  for (const bool &v: fixedVars) {
      std::cout << (int) v << ", ";
  }
  std::cout << std::endl;

  std::cout << "Unfixed: " << std::endl;
  for (const VariableDomain &v: unfixedVars) {
      v.print();
  }
  std::cout << "---------------" << std::endl;

  for (const EqConstraintKey &c: constraints) {
      std::cout << "[ (" << c.id << ") ";
      for (const std::optional<int> &i: c.values) {
          std::cout << ( i.has_value() ? i.value() : -1) << ", ";
      }
      std::cout << "]" << std::endl;
  }*/
}
