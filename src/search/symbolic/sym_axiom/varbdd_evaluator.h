#ifndef SEARCH_SYMBOLIC_SYM_AXIOM_VARBDD_EVALUATOR_H_
#define SEARCH_SYMBOLIC_SYM_AXIOM_VARBDD_EVALUATOR_H_

#include "axiom_evaluator.h"

namespace symbolic
{

class VarBddEvaluator : public SymAxiomEvaluatorTR<SymAxiom>
{
  protected:
	std::vector<int> axiom_body_layer;
	std::map<int, BDD> var_bdds;
        
	void create_axiom_body_layer();
        void create_derived_var_bdds();
	void create_derived_var_bdds(int layer);
	void get_body_bdd(int axiom_id, BDD &res, int skip_var = -1) const;

  public:
VarBddEvaluator(const Options & opts) : SymAxiomEvaluatorTR<SymAxiom>(opts) {
        }

	virtual void init_axioms();
	virtual void apply_axioms(const BDD &bdd, BDD &res) const;
        virtual std::string get_name() const override {
            return "varbdds";
        }
};

} // namespace symbolic

#endif /* SEARCH_SYMBOLIC_SYM_AXIOM_VARBDD_EVALUATOR_H_ */
