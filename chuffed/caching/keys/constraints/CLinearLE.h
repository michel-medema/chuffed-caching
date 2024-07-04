#ifndef CHUFFED_CLINEARLE_H
#define CHUFFED_CLINEARLE_H

#include "DomConstraintKey.h"

class CLinearLE : public DomConstraintKey {
  protected:
    const int64_t a;
    const bool b;

    bool checkDominance( const DomConstraintKey &c ) const override {
      const auto &constraint = dynamic_cast<CLinearLE const &>(c);

      return b == constraint.b && a >= constraint.a;
    }

  public:
    CLinearLE( int id, int64_t a ) : DomConstraintKey( id ), a( a ), b( false ) {}

    CLinearLE( int id, int64_t a, bool b ) : DomConstraintKey( id ), a( a ), b( b ) {}

    size_t size() const override {
      return sizeof(*this);
    }

    void print() const override {
      std::cout << "(lin " << id << ") " << a << ", " << b << std::endl;
    }
};

#endif //CHUFFED_CLINEARLE_H
