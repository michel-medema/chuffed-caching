#ifndef CHUFFED_TABLE_H
#define CHUFFED_TABLE_H

#include <vector>

#include "EquivalenceConstraint.h"

class Table : public EquivalenceConstraint {
  private:
    std::vector<IntVar *> xs;
    std::vector<BoolView> booleans;
    bool b = false;

  public:
    explicit Table( std::vector<IntVar *> xs, std::vector<BoolView> bools ) : xs( std::move( xs ) ), booleans( std::move( bools ) ),
                                                                                     EquivalenceConstraint(
                                                                                       xs.size() + bools.size() ) {
      for ( int i = 0; i < this->xs.size(); i++ ) {
        this->xs[i]->attach( this, i, EVENT_F );
      }

      for ( int i = 0; i < this->booleans.size(); i++ ) {
        if ( this->booleans[i].isFixed() ) {
          fixed++;
          if ( this->booleans[i].isTrue() ) b = true;
        } else {
          this->booleans[i].attach( this, i + this->xs.size(), EVENT_L | EVENT_U );
          engine.addBool( this->booleans[i] );
        }
      }
    }

    void wakeup( int i, int c ) override {
      if ( i < xs.size() && c & EVENT_F ) {
        fixed++;
      }

      if ( i >= xs.size() ) {
        fixed++;
        if ( booleans[i - xs.size()].isTrue() ) b = true;
      }
    }

    bool propagate() override {
      return true;
    }

    void projectionKey( std::vector<int64_t>& ints, std::vector<bool>& bools ) const override {
      bools.emplace_back( b );

      for ( const BoolView &boolean: booleans ) {
        bools.emplace_back( boolean.isFixed() );
      }

      /*std::vector<bool> fixed;
      fixed.reserve( bools.size() );

      for ( const BoolView &boolean: bools ) {
        fixed.emplace_back( boolean.isFixed() );
      }

      return std::make_unique<CTable>( prop_id, std::move( fixed ), b );*/
    }

  protected:
    std::vector<int> scope() const override {
      std::vector<int> scope;
      scope.reserve( xs.size() );

      for ( const IntVar *v: xs ) {
        scope.push_back( v->var_id );
      }

      return scope;
    }

};

#endif //CHUFFED_TABLE_H
