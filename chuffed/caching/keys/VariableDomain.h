#ifndef CHUFFED_VARIABLEDOMAIN_H
#define CHUFFED_VARIABLEDOMAIN_H

#include <vector>
#include <iostream>
#include <cmath>
#include <memory>

#include "chuffed/caching/hash.h"
#include "chuffed/caching/key-size.h"

class IntVar;

class VariableDomain {
  private:
    std::vector<bool> values;

  public:
    const int id;
    const int min;
    const int domainSize;
    //std::vector<uint8_t> values;
    //std::vector<uint8_t> values = {0, 0, 0};
    //uint8_t *values;

    VariableDomain( int id, int min, int domainSize, std::vector<bool> values );

    bool operator==( const VariableDomain &v ) const;

    const std::vector<bool>& getValues() const {
      return this->values;
    }

    bool dominates( const VariableDomain &v ) const;

    void print() const {
      /*std::cout << "[ (" << id << ") " << min << " - ";
      for ( const bool &b: values ) {
        std::cout << (int) b << ", ";
      }
      std::cout << "]" << std::endl;*/
    }

    std::size_t hash() const {
      size_t hash = 0;

      hashCombine( hash, id );
      hashCombine( hash, min );
      hashCombine( hash, values );

      /*for ( const uint8_t &v: values ) {
        hashCombine( hash, v );
      }*/

      return hash;
    }

    size_t size() const {
      //return sizeof(*this) + (this->values.capacity() / 8) + 1;

      // (sizeof(uint8_t) * maxSize);
      return sizeof(*this) + numBytes(this->values); //+ (this->values.capacity() * sizeof(decltype(this->values)::value_type));
    }

    static VariableDomain getDomain( const IntVar *var );

    static bool completeDomain( const IntVar *var );

    static bool originalDomain( const IntVar *var );
};

#endif //CHUFFED_VARIABLEDOMAIN_H
