#ifndef SEARCH_SYMBOLIC_SYM_AXIOM_AXIOM_EVALUATOR_H_
#define SEARCH_SYMBOLIC_SYM_AXIOM_AXIOM_EVALUATOR_H_

#include "../sym_enums.h"
#include "../../utils/timer.h"
#include "axiom_tr.h"
#include "stratification.h"
#include "../../option_parser_util.h"
#include "../../option_parser.h"

namespace symbolic {
class SymVariables;
class SymParamsMgr;

class SymAxiomEvaluator {
protected:
	SymVariables *vars; // For axiom creation
	const SymParamsMgr *p; // Contains parameters
	mutable double eval_time;

        std::unique_ptr<AxiomStratification> axiom_stratification;
        std::unique_ptr<VariableStratification> variable_stratification;
public:
        SymAxiomEvaluator(const Options & opts);
        
	void init(SymVariables *v, const SymParamsMgr *params);

	virtual void init_axioms() = 0;
	virtual void apply_axioms(const BDD &bdd, BDD &res) const = 0;
	virtual void process_init_state(const BDD &bdd, BDD &res) const;
	virtual void process_goal_state(const BDD &bdd, BDD &res) const;
	virtual BDD derived_representation(int var, int val) const;

	virtual void abstract_derived(const BDD &bdd, BDD &res) const = 0;
	virtual void set_derived_to_default(const BDD &bdd, BDD &res) const = 0;

        virtual std::string get_name() const = 0;

        static void add_options_to_parser(OptionParser & parser);

        bool is_derived_variable(int var) const {
            return variable_stratification->is_derived_variable(var);
        }
	static int get_axiom_head(int axiom_id);
	static bool is_in_body(int var, int axiom_id);
	static bool is_trivial(int axiom_id); // Axiom sets head variable to default => not necessary!
};



template<typename T> 
    class SymAxiomEvaluatorTR : public SymAxiomEvaluator { 

protected:
    // Auxilary structured which can be used by the evaluators after initalizing them
    BDD derived_cube;
    BDD derived_default_values;
    std::vector<std::vector<T>> axioms;    // Axioms organized per layers
    std::vector<std::vector<BDD>> removers;// Negated precondition

    // Auxillary functions which can be used by the evaluators
    void create_axioms_trs();
    void create_abstract_cube();
    void create_axioms_defaults();
    void merge_axioms();

public:
    SymAxiomEvaluatorTR(const Options & opts) : SymAxiomEvaluator(opts) {
    }

    virtual void abstract_derived(const BDD &bdd, BDD &res) const;
    virtual void set_derived_to_default(const BDD &bdd, BDD &res) const;

};

}

#endif /* SEARCH_SYMBOLIC_SYM_AXIOM_EVALUATOR_H_ */
