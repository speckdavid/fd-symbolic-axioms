#include "axiom_evaluator.h"
#include <limits>
#include <queue>
#include "../../search_engines/symbolic_search.h"
#include "../sym_state_space_manager.h"
#include "../sym_util.h"
#include "interleaved_evaluator.h"
#include "interleaved_evaluator_axiomrule.h"
#include "precomputed_evaluator.h"
#include "stratification.h"
#include "transform_evaluator.h"
#include "varbdd_evaluator.h"
#include "axiom_rule.h"
#include "../../plugin.h"
using namespace std;

namespace symbolic {
    SymAxiomEvaluator::SymAxiomEvaluator(const Options & opts) {
        auto stratifier = opts.get<shared_ptr<Stratifier>>("axiom_stratification");

        variable_stratification = stratifier->get_variable_stratification();
        axiom_stratification = stratifier->get_axiom_stratification();
    }

    void SymAxiomEvaluator::add_options_to_parser(OptionParser & parser) {
        parser.add_option<shared_ptr<Stratifier>>("axiom_stratification",  "axiom stratification", "default");
    }
// std::shared_ptr<SymAxiomEvaluator> SymAxiomEvaluator::get_sym_axiom_evaluator(
//     SymVariables *v, const SymParamsMgr *params) {
//   // if (params->axiom_stratification == AxiomStratification::TRANSITIVE_CLOSURE) {
//   //   std::cout << "Creating stratification...";
//   //   stratifier.stratify_transitive_closure(var_layers);
//   //   std::cout << "done." << std::endl;
//   // } else {
//   //   var_layers = g_axiom_layers;
//   // }
//   // assert(stratifier.is_stratified(var_layers));
    
//   std::shared_ptr<SymAxiomEvaluator> evaluator;
//   if (params->axiom_type == AxiomType::INTERLEAVED) {
//     if (symbolic_search::g_symbolic_search_direction !=
//         symbolic_search::SearchDirection::FWD) {
//       throw std::invalid_argument(
//           "Axiom type is only with forward symbolic search supported.");
//     }
//     evaluator = std::make_shared<InterleavedEvaluator>(v, params);
//   } else if (params->axiom_type == AxiomType::PRECOMPUTED) {
//     if (symbolic_search::g_symbolic_search_direction !=
//         symbolic_search::SearchDirection::FWD) {
//       throw std::invalid_argument(
//           "Axiom type is only with forward symbolic search supported.");
//     }
//     evaluator = std::make_shared<PrecomputedEvaluator>(v, params);
//   } else if (params->axiom_type == AxiomType::VARBDDS) {
//     if (symbolic_search::g_symbolic_search_direction !=
//         symbolic_search::SearchDirection::FWD) {
//       throw std::invalid_argument(
//           "Axiom type is only with forward symbolic search supported.");
//     }
//     evaluator = std::make_shared<VarBddEvaluator>(v, params);
//   } else if (params->axiom_type == AxiomType::TRANSFORM) {
//     evaluator = std::make_shared<TransformEvaluator>(v, params);
//   } else if (params->axiom_type == AxiomType::INTERLEAVED_RULES) {
//     if (symbolic_search::g_symbolic_search_direction !=
//         symbolic_search::SearchDirection::FWD) {
//       throw std::invalid_argument(
//           "Axiom type is only with forward symbolic search supported.");
//     }
//     evaluator = std::make_shared<InterleavedEvaluatorAxiomRule>(v, params);
//   } else {
//     throw std::invalid_argument("Axiom type is not supported.");
//   }
//   return evaluator;
// }

// Some helper functions

int SymAxiomEvaluator::get_axiom_head(int axiom_id) {
  return g_axioms[axiom_id].get_effects()[0].var;
}

bool SymAxiomEvaluator::is_in_body(int var, int axiom_id) {
  for (auto &pre : g_axioms[axiom_id].get_effects()[0].conditions) {
    if (pre.var == var) {
      return true;
    }
  }
  return false;
}

// Axiom sets head variable to default => not necessary!
bool SymAxiomEvaluator::is_trivial(int axiom_id) {
  int head = get_axiom_head(axiom_id);
  return g_axioms[axiom_id].get_effects()[0].val ==
         g_default_axiom_values[head];
}

// Class specific functions
void SymAxiomEvaluator::process_init_state(const BDD &bdd, BDD &res) const {
  // vars->to_dot(bdd, "init_before.dot");
  apply_axioms(bdd, res);
  // vars->to_dot(res, "init.dot");
  // exit(0);
}
void SymAxiomEvaluator::process_goal_state(const BDD &bdd, BDD &res) const {
  res = bdd;
}

BDD SymAxiomEvaluator::derived_representation(int var, int val) const {
  return vars->preBDD(var, val);
}

    void SymAxiomEvaluator::init(SymVariables *v,
                                 const SymParamsMgr *params) {
        vars = v;
        p = params;
        eval_time = 0;
        
    }

    template <typename T>
    void SymAxiomEvaluatorTR<T>::create_axioms_trs() {
        axioms.resize(axiom_stratification->get_num_layers());
        removers.resize(axiom_stratification->get_num_layers());
        for (size_t i = 0; i < g_axioms.size(); i++) {
            if (is_trivial(i)) {
                continue;
            }
            const GlobalOperator *ax = &(g_axioms[i]);
            assert(ax->get_effects().size() == 1);
            int level = axiom_stratification->get_layer(i);   
            axioms[level].emplace_back(vars, ax, level);
            removers[level].push_back(axioms[level].back().get_remover());
        }

	for (auto & entry :  removers) {
	    merge(vars, entry, mergeAndBDD, p->max_tr_time, p->max_tr_size);
        }
    }

    template <typename T>
    void SymAxiomEvaluatorTR<T>::create_abstract_cube() {
        // Build cube to delete values of dervied values (quantify)
        derived_cube = vars->mgr()->bddOne();
        for (int v : variable_stratification->get_derived_vars()) {
            derived_cube *= vars->getCubePre(v);
        }
    }

    template <typename T>
    void SymAxiomEvaluatorTR<T>::create_axioms_defaults() {
        // Create default values
        std::vector<std::pair<int, int>> axioms_default;
        derived_default_values = vars->oneBDD();
        for (int v : variable_stratification->get_derived_vars()) {
            derived_default_values *= vars->preBDD(v, g_default_axiom_values[v]);
        }
    }


    template <typename T>
    void SymAxiomEvaluatorTR<T>::merge_axioms() {
        for (auto & entry :  axioms) {
            merge(vars, entry, mergeAxiom, p->max_tr_time, p->max_tr_size);
        }
    }

    template <>
    void SymAxiomEvaluatorTR<SymAxiomRule>::merge_axioms() {
	for (size_t i = 0; i < axioms.size(); ++i) {
	    std::map<int, SymAxiomRule> merged_by_effect;
	    vector<SymAxiomRule> non_merged;

	    for (const SymAxiomRule & r : axioms[i]) {
		assert(r.getEffectVars().size() == 1);
		int effect_var = r.getEffectVars()[0];
		if (merged_by_effect.count(effect_var)) {
		    try { 
			merged_by_effect.at(effect_var).merge(r, p->max_tr_size);
		    } catch(BDDError ) {
			non_merged.push_back(r);
		    }
		} else {
		    merged_by_effect.insert(std::map<int, SymAxiomRule>::value_type(effect_var, r));
		}
	    }

	    vector<SymAxiomRule> merged_same_effect;
	    for (const auto & entry : merged_by_effect) {
		merged_same_effect.push_back(entry.second);
	    }
	    merge(vars, merged_same_effect, mergeAxiomRule, p->max_tr_time, p->max_tr_size);
	    axioms[i] = merged_same_effect;
	    for (const auto & r : non_merged) {
		axioms[i].push_back(r);
	    }
	}
    }


     template <typename T>
    void SymAxiomEvaluatorTR<T>::abstract_derived(const BDD &bdd, BDD &res) const {
  // TODO (speckd): Fix max number of nodes
  res = bdd.ExistAbstract(derived_cube, std::numeric_limits<int>::max());
}

     template <typename T>
     void SymAxiomEvaluatorTR<T>::set_derived_to_default(const BDD &bdd, BDD &res) const {
  BDD result;
  abstract_derived(bdd, result);
  result *= derived_default_values;
  res = result;
}
    template class SymAxiomEvaluatorTR<SymAxiom>;
    template class SymAxiomEvaluatorTR<SymAxiomRule>;




    

static PluginTypePlugin<SymAxiomEvaluator> _type_plugin(
    "SymAxiomEvaluator",
    "axiom evaluator");

}  // namespace symbolic
