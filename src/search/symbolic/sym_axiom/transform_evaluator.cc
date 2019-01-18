#include "transform_evaluator.h"
#include <limits>
#include <queue>

#include "../../plugin.h"
#include "../option_parser.h"

namespace symbolic {
    using namespace std;

    void TransformEvaluator::init_axioms() {
        utils::Timer timer;
        create_derived_var_bdds();

        std::cout << std::fixed << "Symbolic Axiom initialization: " << timer
                  << std::endl;
    }

    void TransformEvaluator::apply_axioms(const BDD & /*bdd*/, BDD & /*res*/) const {}

    void TransformEvaluator::process_init_state(const BDD & /*bdd*/,
                                                BDD &res) const {
        res = vars->oneBDD();
        for (size_t var = 0; var < g_initial_state_data.size(); var++) {
            int val = g_initial_state_data[var];
            if (!variable_stratification->is_derived_variable(var)) {
                res *= vars->preBDD(var, val);
            }
            /*else
              {
              res *= derived_representation(var, val);
              }*/
        }
    }

    void TransformEvaluator::process_goal_state(const BDD & /*bdd*/,
                                                BDD &res) const {
        res = vars->oneBDD();
        for (auto &var_val : g_goal) {
            if (variable_stratification->is_derived_variable(var_val.first)) {
                res *= derived_representation(var_val.first, var_val.second);
            } else {
                res *= vars->preBDD(var_val.first, var_val.second);
            }
        }
    }

    BDD TransformEvaluator::derived_representation(int var, int val) const {
        BDD res = var_bdds.at(var);

        // Negation because default derived variable has default value
        return g_default_axiom_values[var] == val ? !res : res;
    }

    void TransformEvaluator::abstract_derived(const BDD &bdd, BDD &res) const {
        res = bdd;
    }

    void TransformEvaluator::set_derived_to_default(const BDD &bdd,
                                                    BDD &res) const {
        res = bdd;
    }



    static shared_ptr<SymAxiomEvaluator> _parse_axiom_evaluator_transform(OptionParser &parser) {
        SymAxiomEvaluator::add_options_to_parser(parser);
        Options opts = parser.parse();
        if (parser.dry_run())
            return nullptr;
        else
            return make_shared<TransformEvaluator>(opts);
    }


    static PluginShared<SymAxiomEvaluator> _plugin("transform", _parse_axiom_evaluator_transform);
    

}  // namespace symbolic
