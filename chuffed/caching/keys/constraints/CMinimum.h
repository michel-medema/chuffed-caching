#ifndef CHUFFED_CMINIMUM_H
#define CHUFFED_CMINIMUM_H

#include "DomConstraintKey.h"

class CMinimum: public DomConstraintKey {
    // The constraint c ≡ y = min xi.

    public:
        // Lower bound on the x values.
        const int64_t e;

        // An upper bound on y.
        const int64_t d;

        CMinimum(int id, int64_t e, int64_t d): DomConstraintKey(id), e(e), d(d) {}

        size_t size() const override {
          return sizeof(*this);
        }

        void print() const override {
            std::cout << "(min " << id << ") " << e << ", " << d << std::endl;
        }

  protected:
    bool checkDominance(const DomConstraintKey& c) const override {
      const auto &constraint = dynamic_cast<CMinimum const &>(c);

      return e <= constraint.e && d >= constraint.d;
    }
};

#endif //CHUFFED_CMINIMUM_H
