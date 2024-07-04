#ifndef CHUFFED_BOOLEAN_H
#define CHUFFED_BOOLEAN_H

#include <utility>
#include <memory>

#include "EquivalenceConstraint.h"


class Boolean : public EquivalenceConstraint {
  private:
    std::vector<BoolView> bools;

  public:
    explicit Boolean( std::vector<BoolView> bools ) : EquivalenceConstraint( bools.size(), false ),
                                                      bools( std::move( bools ) ) {
      engine.booleanConstraints.push_back( this );

      for ( int i = 0; i < this->bools.size(); ++i ) {
        if ( this->bools[i].isFixed() ) {
          ++fixed;

          if ( this->bools[i].isTrue() ) satisfied = true;
        } else {
          this->bools[i].attach( this, i, EVENT_L | EVENT_U );

          engine.addBool( this->bools[i] );
        }
      }
    }

    void wakeup( int i, int c ) override {
      ++fixed;

      if ( bools[i].isTrue() ) {
        satisfied = true;
      }
    }

    bool propagate() override {
      return true;
    }

    // TODO: This method is not used for these Boolean constraints.
    void projectionKey( std::vector<int64_t>& ints, std::vector<bool>& booleans ) const override {
      booleans.emplace_back( satisfied );
      //return std::make_unique<CBoolean>( prop_id, satisfied );
    }

  protected:
    std::vector<int> scope() const override {
      return std::vector<int>{};
    }
};

#endif //CHUFFED_BOOLEAN_H