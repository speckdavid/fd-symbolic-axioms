#include "precomputed_evaluator.h"
#include <limits>

namespace symbolic {

void PrecomputedEvaluator::init_axioms() {
	utils::Timer timer;
	create_abstract_cube();
	create_axioms_trs();
	create_axioms_defaults();
	merge_axioms();
	valid_states = vars->mgr()->bddOne();
	std::cout << "Big fix point..." << std::endl;

	// Initally we need fixpoint evaluation once
	apply_fixpoint(valid_states, valid_states);
	std::cout << "done!" << std::endl;
	std::cout << std::fixed << "Symbolic Axiom initialization: " << timer << std::endl;
}

void PrecomputedEvaluator::apply_axioms(const BDD &bdd, BDD &res) const {
	utils::Timer timer;
	BDD result;
	abstract_derived(bdd, result);
	res = result * valid_states;
	eval_time += timer.stop();
	std::cout << std::fixed <<"    => Axiom evaluation took " << timer << " [Overall: " << eval_time << "s" << "]" << std::endl;
}

}

