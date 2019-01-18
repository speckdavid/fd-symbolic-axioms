#ifndef SEARCH_SYMBOLIC_SYM_AXIOM_INTERLEAVED_EVALUATOR_H_
#define SEARCH_SYMBOLIC_SYM_AXIOM_INTERLEAVED_EVALUATOR_H_

#include "axiom_evaluator.h"

namespace symbolic {

    class InterleavedEvaluator : public SymAxiomEvaluatorTR<SymAxiom> {
    public:
    InterleavedEvaluator(const Options & opts) : SymAxiomEvaluatorTR<SymAxiom> (opts) {
        }

	InterleavedEvaluator(SymVariables *v, const SymParamsMgr *params);
	virtual void init_axioms();
	virtual void apply_axioms(const BDD &bdd, BDD &res) const;

        virtual std::string get_name() const override {
            return "interleaved";
        }
    protected:
	void apply_fixpoint(const BDD &bdd, BDD &res) const;
    };

}



#endif /* SEARCH_SYMBOLIC_SYM_AXIOM_INTERLEAVED_EVALUATOR_H_ */
