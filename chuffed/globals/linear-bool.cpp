#include "chuffed/core/engine.h"
#include "chuffed/core/propagator.h"
#include "chuffed/core/sat-types.h"
#include "chuffed/primitives/primitives.h"
#include "chuffed/support/misc.h"
#include "chuffed/support/vec.h"
#include "chuffed/vars/bool-view.h"
#include "chuffed/vars/int-var.h"
#include "chuffed/vars/int-view.h"
#include "chuffed/vars/vars.h"

#include "chuffed/caching/keys/constraints/CLinearLE.h"
#include "chuffed/caching/propagators/DominanceConstraint.h"

// sum x_i <= y

template <int U = 0>
class BoolLinearLE : public DominanceConstraint {
	public:
		vec<BoolView> x;
		IntView<U> y;

		// Persistent state
		Tint ones;

		vec<Lit> ps;

		BoolLinearLE(vec<BoolView>& _x, IntView<U> _y) : DominanceConstraint( _x.size() + 1 ), x(_x), y(_y), ones(0) {
			for (int i = 0; i < x.size(); i++) {
				x[i].attach(this, i, EVENT_L | EVENT_U);
			}
			y.attach(this, x.size(), EVENT_U | EVENT_F);

			for (int i = 0; i < x.size(); i++) {
				if ( !x[i].isFixed() ) {
					engine.addBool( x[i] );
				}
			}
		}

		void wakeup(int i, int c) override {
			if ( i < x.size() ) {
				fixed++;

				if ( x[i].isTrue() ) {
					ones++;
					pushInQueue();
				}
			} else {
				if ( (c & y.getEvent(EVENT_U)) != 0 ) { pushInQueue(); }
				if ( (c & EVENT_F) != 0 ) { fixed++; }
			}
		}

		bool propagate() override {
			const int y_max = y.getMax();

			if (ones > y_max) {
				ones = y_max + 1;
			}

			setDom2(y, setMin, ones, 1);

			if (ones == y_max) {
				for (int i = 0; i < x.size(); i++) {
					if (!x[i].isFixed()) {
						x[i].setVal2(false, Reason(prop_id, 0));
					}
				}
			}

			return true;
		}

		Clause* explain(Lit /*p*/, int inf_id) override {
			ps.clear();
			ps.growTo(ones + 1);
			for (int i = 0, j = 1; j <= ones; i++) {
				if (x[i].isTrue()) {
					ps[j++] = ~x[i];
				}
			}
			if (inf_id == 0) {
				ps.push(y.getMaxLit());
			}
			return Reason_new(ps);
		}

		std::unique_ptr<DomConstraintKey> projectionKey() const override {
			return std::make_unique<CLinearLE>( CLinearLE(prop_id, y.getMax() - ones) );
		}

	protected:
		std::vector<int> scope() const override {
			return std::vector<int>{ y.var->var_id };
		}
};

//-----

// sum x_i (=, <=, <, >=, >) y

void bool_linear(vec<BoolView>& x, IntRelType t, IntVar* y) {
	vec<BoolView> x2;
	for (int i = 0; i < x.size(); i++) {
		x2.push(~x[i]);
	}
	switch (t) {
		case IRT_EQ:
			// sum x_i = y <=> sum x_i <= y /\ sum (1-x_i) <= (-y+x.size())
			new BoolLinearLE<0>(x, IntView<0>(y));
			new BoolLinearLE<5>(x2, IntView<5>(y, 1, x.size()));
			break;
		case IRT_LE:
			// sum x_i <= y
			new BoolLinearLE<0>(x, IntView<0>(y));
			break;
		case IRT_LT:
			// sum x_i < y <=> sum x_i <= (y-1)
			new BoolLinearLE<4>(x, IntView<4>(y, 1, -1));
			break;
		case IRT_GE:
			// sum x_i >= y <=> sum (1-x_i) <= (-y+x.size())
			new BoolLinearLE<5>(x2, IntView<5>(y, 1, x.size()));
			break;
		case IRT_GT:
			// sum x_i > y <=> sum (1-x_i) <= (-y+x.size()-1)
			new BoolLinearLE<5>(x2, IntView<5>(y, 1, x.size() - 1));
			break;
		default:
			CHUFFED_ERROR("Unknown IntRelType %d\n", t);
	}
}
