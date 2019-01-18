#ifndef SEARCH_SYMBOLIC_SYM_AXIOM_INTERLEAVED_EVALUATOR_AXIOMRULE_H_
#define SEARCH_SYMBOLIC_SYM_AXIOM_INTERLEAVED_EVALUATOR_AXIOMRULE_H_

#include "axiom_evaluator.h"
#include "axiom_rule.h"

namespace symbolic {

class InterleavedEvaluatorAxiomRule : public SymAxiomEvaluatorTR<SymAxiomRule> {
public:

        InterleavedEvaluatorAxiomRule(const Options & opts) : SymAxiomEvaluatorTR<SymAxiomRule> (opts) {
        }

	virtual void init_axioms();
	virtual void apply_axioms(const BDD &bdd, BDD &res) const;

        virtual std::string get_name() const override {
            return "interleaved_rules";
        }

protected:
	void apply_fixpoint(const BDD &bdd, BDD &res) const;
};

}



#endif /* SEARCH_SYMBOLIC_SYM_AXIOM_INTERLEAVED_EVALUATOR_H_ */
 
