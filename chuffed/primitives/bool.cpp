#include "chuffed/core/sat-types.h"
#include "chuffed/core/sat.h"
#include "chuffed/primitives/primitives.h"
#include "chuffed/support/misc.h"
#include "chuffed/support/vec.h"
#include "chuffed/vars/bool-view.h"

#include "chuffed/caching/propagators/Boolean.h"

#include <utility>

void bool_rel(BoolView x, BoolRelType t, BoolView y, BoolView z) {
	//	NOT_SUPPORTED;
	const BoolView v[3] = {std::move(x), std::move(y), std::move(z)};
	int u = 0;

	// Add boolean variables to engine for caching.
	engine.addBool( x );
	engine.addBool( y );
	engine.addBool( z );

	for (int l = 1; l <= 3; l++) {
		for (int i = 0; i < 8; i++) {
			if (bitcount<int>(i) != l) {
				continue;
			}
			for (int j = 0; j < 8; j++) {
				if ((j & ~i) != 0) {
					continue;
				}
				int flags = 0;
				for (int k = 0; k < 8; k++) {
					bool pass = true;
					for (int m = 0; m < 3; m++) {
						if ((i & (1 << m)) == 0) {
							continue;
						}
						if (((k ^ j) & (1 << m)) == 0) {
							pass = false;
						}
					}
					if (pass) {
						flags |= (1 << k);
					}
				}
				if ((t & flags) != 0) {
					continue;
				}
				if ((flags & ~u) == 0) {
					continue;
				}
				vec<Lit> ps;
				for (int m = 0; m < 3; m++) {
					if ((i & (1 << m)) == 0) {
						continue;
					}
					const bool p = ((j >> m) & 1) != 0;
					if (l == 1) {
						if (v[m].setValNotR(p)) {
							if (!v[m].setVal(p)) {
								TL_FAIL();
							}
						}
					}
					ps.push(v[m].getLit(p));
				}
				if (l >= 2) {
					sat.addClause(ps);
				}
				u |= flags;
			}
		}
	}
}

//-----

// \/ x_i \/ !y_i
void bool_clause(vec<BoolView>& x, vec<BoolView>& y) {
	vec<Lit> ps;
	for (int i = 0; i < x.size(); i++) {
		ps.push(x[i]);
	}
	for (int i = 0; i < y.size(); i++) {
		ps.push(~y[i]);
	}
	sat.addClause(ps);

	// Add Boolean propagator for caching.
	std::vector<BoolView> lits;
	lits.reserve( x.size() + y.size() );

	for (int i = 0; i < x.size(); ++i) { lits.push_back( x[i] ); }
	for (int i = 0; i < y.size(); ++i) { lits.push_back( ~y[i] ); }

	new Boolean( lits);
}

// \/ x_i
void bool_clause(vec<BoolView>& x) {
	vec<BoolView> b;
	bool_clause(x, b);
}

//-----

// \/ x_i \/ !y_i <-> z
// n+1 clauses

void array_bool_or(vec<BoolView>& x, vec<BoolView>& y, BoolView z) {
	for (int i = 0; i < x.size(); i++) {
		sat.addClause(~x[i], z);
	}
	for (int i = 0; i < y.size(); i++) {
		sat.addClause(y[i], z);
	}
	// Add clause !c \/ a_i \/ !b_i
	vec<Lit> ps;
	ps.push(~z);
	for (int i = 0; i < x.size(); i++) {
		ps.push(x[i]);
	}
	for (int i = 0; i < y.size(); i++) {
		ps.push(~y[i]);
	}
	sat.addClause(ps);

	// Add Boolean propagator for caching.
	std::vector<BoolView> lits;
	lits.reserve( x.size() + y.size() + 1 );

	for (int i = 0; i < x.size(); ++i) { lits.push_back( x[i] ); }
	for (int i = 0; i < y.size(); ++i) { lits.push_back( ~y[i] ); }
	lits.push_back( ~z );

	new Boolean( lits);
}

void array_bool_or(vec<BoolView>& x, BoolView z) {
	vec<BoolView> y;
	array_bool_or(x, y, std::move(z));
}

// /\ x_i /\ !y_i <-> z
void array_bool_and(vec<BoolView>& x, vec<BoolView>& y, BoolView z) { array_bool_or(y, x, ~z); }

void array_bool_and(vec<BoolView>& x, BoolView z) {
	vec<BoolView> y;
	array_bool_and(x, y, std::move(z));
}
