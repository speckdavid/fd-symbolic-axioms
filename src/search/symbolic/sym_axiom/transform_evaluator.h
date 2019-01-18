#ifndef SEARCH_SYMBOLIC_SYM_AXIOM_TRANSFORM_EVALUATOR_H_
#define SEARCH_SYMBOLIC_SYM_AXIOM_TRANSFORM_EVALUATOR_H_

#include "axiom_evaluator.h"
#include "varbdd_evaluator.h"

namespace symbolic
{

class TransformEvaluator : public VarBddEvaluator
{
  public:
TransformEvaluator(const Options & opts) : VarBddEvaluator(opts) {
    }

	
	void init_axioms();
	void apply_axioms(const BDD &bdd, BDD &res) const;
	void process_init_state(const BDD &bdd, BDD &res) const;
	void process_goal_state(const BDD &bdd, BDD &res) const;

	BDD derived_representation(int var, int val) const;
	void abstract_derived(const BDD &bdd, BDD &res) const;
	void set_derived_to_default(const BDD &bdd, BDD &res) const;

        virtual std::string get_name() const override {
            return "transform";
        }
};

} // namespace symbolic

#endif /* SEARCH_SYMBOLIC_SYM_AXIOM_TRANSFORM_EVALUATOR_H_ */
