#ifndef SEARCH_SYMBOLIC_SYM_AXIOM_PRECOMPUTED_EVALUATOR_H_
#define SEARCH_SYMBOLIC_SYM_AXIOM_PRECOMPUTED_EVALUATOR_H_

#include "interleaved_evaluator.h"

namespace symbolic {

class PrecomputedEvaluator : public InterleavedEvaluator {
protected:
	BDD valid_states;

public:
	PrecomputedEvaluator(SymVariables *v, const SymParamsMgr *params);
	void init_axioms();
	void apply_axioms(const BDD &bdd, BDD &res) const;
};

}



#endif /* SEARCH_SYMBOLIC_SYM_AXIOM_PRECOMPUTED_EVALUATOR_H_ */
