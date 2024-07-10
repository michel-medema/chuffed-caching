#ifndef CHUFFED_BOOLEQREIF_H
#define CHUFFED_BOOLEQREIF_H

#include "EquivalenceConstraint.h"
#include "chuffed/vars/bool-view.h"
#include "chuffed/vars/vars.h"

class BoolEqReif: public EquivalenceConstraint {
	private:
		const BoolView x;
		const BoolView y;
		const BoolView z;

	public:
		BoolEqReif( const BoolView& x, const BoolView& y, const BoolView& z ): EquivalenceConstraint(3), x(x), y(y), z(z) {
			engine.addBool( x );
			engine.addBool( y );
			engine.addBool( z );

			if ( x.isFixed() || y.isFixed() || z.isFixed() ) {
				keyIsTrue = true;
			} else {
				x.attach( this, 0, EVENT_L | EVENT_U );
				y.attach( this, 1, EVENT_L | EVENT_U );
				z.attach( this, 2, EVENT_L | EVENT_U );
			}
		}

		void wakeup( int  /*i*/, int /*c*/ ) override {
			++fixed;

			if ( fixed > 1 ) { // If at least two of the booleans are fixed, the value of the third can be derived.
				satisfied = true;
			}
		}

		bool propagate() override {
			return true;
		}

		void projectionKey( std::vector<int64_t>& ints, std::vector<bool>& booleans ) const override {
			if ( x.isFixed() ) {
				booleans.emplace_back( x.isTrue() );
			}
			if ( y.isFixed() ) {
				booleans.emplace_back( y.isTrue() );
			}
			if ( z.isFixed() ) {
				booleans.emplace_back( z.isTrue() );
			}
		}
};

#endif  // CHUFFED_BOOLEQREIF_H
