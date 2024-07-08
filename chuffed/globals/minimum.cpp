#include "chuffed/core/engine.h"
#include "chuffed/core/options.h"
#include "chuffed/core/propagator.h"
#include "chuffed/core/sat-types.h"
#include "chuffed/core/sat.h"
#include "chuffed/support/vec.h"
#include "chuffed/vars/int-var.h"
#include "chuffed/vars/int-view.h"
#include "chuffed/vars/vars.h"

#include "chuffed/caching/propagators/DominanceConstraint.h"
#include "chuffed/caching/keys/constraints/CMinimum.h"

#include <climits>
#include <cstdint>

// y = min(x_i)
// Bounds propagator, not fully consistent

template <int U>
class Minimum : public DominanceConstraint, public Checker {
	public:
		const int sz;
		IntView<U>* const x;
		const IntView<U> y;

		// Persistent state
		Tint min_max_var;
		Tint64_t min_max;
		Tint64_t min_fixed;

		// Intermediate state
		bool lower_change{false};

		Minimum(vec<IntView<U> > _x, IntView<U> _y)
				: DominanceConstraint( _x.size() + 1 ), sz(_x.size()),
					x(_x.release()),
					y(_y),
					min_max_var(-1),
					min_max(INT_MAX),
					min_fixed(INT_MAX) {
			priority = 1;
			for (int i = 0; i < sz; i++) {
				x[i].attach(this, i, EVENT_LU | EVENT_F);
			}
			y.attach(this, sz, EVENT_L | EVENT_F);
		}

		void wakeup(int i, int c) override {
			if (i < sz) {
				if ((c & EVENT_F) != 0) {
					const int64_t m = x[i].getVal();
					if (m < min_fixed) {
						min_fixed = m;
					}
				}

				const int64_t m = x[i].getMax();
				if (m < min_max) {
					min_max_var = i;
					min_max = m;
					pushInQueue();
				}
			}

			if (c & y.getEvent(EVENT_L)) {
				lower_change = true;
				pushInQueue();
			}

			if ( (c & EVENT_F) != 0 ) { fixed++; }
		}

		bool propagate() override {
			// make a less than or equal to min(max(x_i))
			setDom(y, setMax, min_max, x[min_max_var].getMaxLit());

			if (lower_change) {
				// make a greater than or equal to min(min(b_i))
				int64_t m = INT64_MAX;
				for (int i = 0; i < sz; i++) {
					const int64_t t = x[i].getMin();
					if (t < m) {
						m = t;
					}
				}
				if (y.setMinNotR(m)) {
					Clause* r = nullptr;
					if (so.lazy) {
						r = Reason_new(sz + 1);
						// Finesse lower bounds
						// Add reason ![y <= m-1] \/ [x_1 <= m-1] \/ ... \/ [x_n <= m-1]
						for (int i = 0; i < sz; i++) {
							(*r)[i + 1] = x[i].getFMinLit(m);
						}
						//					for (int i = 0; i < sz; i++) (*r)[i+1] = x[i].getLit(m-1, LR_LE);
					}
					if (!y.setMin(m, r)) {
						return false;
					}
				}

				// make b_i greater than or equal to min(a)
				m = y.getMin();
				Clause* r = nullptr;
				if (so.lazy) {
					r = Reason_new(2);
					(*r)[1] = y.getMinLit();
				}
				for (int i = 0; i < sz; i++) {
					if (x[i].setMinNotR(m)) {
						if (!x[i].setMin(m, r)) {
							return false;
						}
					}
				}
			}

			// Necessary and sufficient conditions for redundancy

			if (y.getMin() == min_max) {
				satisfied = true;
			}

			return true;
		}

		void clearPropState() override {
			in_queue = false;
			lower_change = false;
		}

		bool check() override {
			int min = INT_MAX;
			for (int i = 0; i < sz; i++) {
				if (x[i].getShadowVal() < min) {
					min = x[i].getShadowVal();
				}
			}
			return (y.getShadowVal() == min);
		}

		int checkSatisfied() override {
			if (satisfied) {
				return 1;
			}
			if (y.getMin() == min_max) {
				satisfied = true;
			}
			return 2;
		}

		std::unique_ptr<DomConstraintKey> projectionKey() const override {
			return std::make_unique<CMinimum>( CMinimum( prop_id, val(y).value_or(INT_MAX), min_fixed ) );
		}

	protected:
		std::vector<int> scope() const override {
			std::vector<int> scope;
			scope.reserve( sz + 1 );

			for (int i = 0; i < sz; i++) {
				scope.push_back(x[i].var->var_id);
			}
			scope.push_back(y.var->var_id);

			return scope;
		}
};

void minimum(vec<IntVar*>& x, IntVar* y) {
	vec<IntView<> > w;
	for (int i = 0; i < x.size(); i++) {
		w.push(IntView<>(x[i]));
	}
	new Minimum<0>(w, IntView<>(y));
}

void maximum(vec<IntVar*>& x, IntVar* y) {
	vec<IntView<> > w;
	for (int i = 0; i < x.size(); i++) {
		w.push(IntView<>(x[i]));
	}
	new Minimum<1>(w, IntView<>(y));
}
