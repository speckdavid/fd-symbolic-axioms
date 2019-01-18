#include "interleaved_evaluator.h"
#include <limits>

#include "../../plugin.h"
#include "../option_parser.h"


namespace symbolic {
    using namespace std;

    void InterleavedEvaluator::init_axioms() {
	utils::Timer timer;
	create_abstract_cube();
	create_axioms_trs();
	create_axioms_defaults();
	merge_axioms();
	std::cout << std::fixed << "Symbolic Axiom initialization: " << timer << std::endl;
    }

    void InterleavedEvaluator::apply_axioms(const BDD &bdd, BDD &res) const {
	utils::Timer timer;
	apply_fixpoint(bdd, res);
	eval_time += timer.stop();
	std::cout << std::fixed <<"    => Axiom evaluation took " << timer << " [Overall: " << eval_time << "s" << "]" << std::endl;
    }

    void InterleavedEvaluator::apply_fixpoint(const BDD &bdd, BDD &res) const {
	BDD cur_states;
	set_derived_to_default(bdd, cur_states);

	// For each layer fixpoint iterations! (their can be gaps)
	for (size_t i = 0; i < axioms.size(); ++i) {
            BDD closed = cur_states;
            BDD cur_res = vars->mgr()->bddZero();
            while (!cur_states.IsZero()) {
                // not applicable
                BDD done_states = cur_states;
                for (const auto &rm : removers[i]) {
                    done_states *= rm;
                }
                cur_res += done_states;
                // Apply axioms
                for (const auto &ax : axioms[i]) {
                    // TODO (speckd): Fix max number of nodes
                    cur_states += ax.image(cur_states,
                                           std::numeric_limits<int>::max());
                }
                // Remove already computed states
                cur_states *= !closed;
                closed += cur_states;
            }
            cur_states = cur_res;
	}
	res = cur_states;
    }


static std::shared_ptr<SymAxiomEvaluator> _parse_axiom_evaluator_transform(OptionParser &parser) {
    SymAxiomEvaluator::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run())
        return nullptr;
    else
        return make_shared<InterleavedEvaluator>(opts);
}


static PluginShared<SymAxiomEvaluator> _plugin("interleaved", _parse_axiom_evaluator_transform);
    



}
