#ifndef CHUFFED_PROJECTIONKEY_H
#define CHUFFED_PROJECTIONKEY_H

#include <utility>

template <class EqKey, class DomKey>
class ProjectionKey {
public:
	const int id;
	const int decisionLevel;
	const int fixedVars;
	const int unfixedVars;
	EqKey equivalencePart;
	DomKey dominancePart;

	ProjectionKey(EqKey equivalencePart, DomKey dominancePart)
			: id(0),
				decisionLevel(0),
				fixedVars(0),
				unfixedVars(0),
				equivalencePart(std::move(equivalencePart)),
				dominancePart(std::move(dominancePart)) {}

	ProjectionKey(int id, EqKey equivalencePart, DomKey dominancePart)
			: id(id),
				decisionLevel(0),
				fixedVars(0),
				unfixedVars(0),
				equivalencePart(std::move(equivalencePart)),
				dominancePart(std::move(dominancePart)) {}

	ProjectionKey(int id, int decisionLevel, int fixedVars, int unfixedVars, EqKey equivalencePart,
								DomKey dominancePart)
			: id(id),
				decisionLevel(decisionLevel),
				fixedVars(fixedVars),
				unfixedVars(unfixedVars),
				equivalencePart(std::move(equivalencePart)),
				dominancePart(std::move(dominancePart)) {}
};

#endif  // CHUFFED_PROJECTIONKEY_H
