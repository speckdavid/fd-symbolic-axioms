#include "varbdd_evaluator.h"
#include <limits>
#include <queue>
#include "../../plugin.h"
#include "../option_parser.h"

namespace symbolic {

    using namespace std;
    void VarBddEvaluator::create_axiom_body_layer() {
        std::set<int> effs;
        for (size_t i = 0; i < g_axioms.size(); i++) {
            int body_level = -1;
            const GlobalOperator *ax = &(g_axioms[i]);
            const GlobalEffect eff = ax->get_effects()[0];
            effs.insert(eff.var);
            for (auto pre : eff.conditions) {
                body_level = std::max(body_level, variable_stratification->get_layer(pre.var));
            }
            axiom_body_layer.push_back(body_level);
        }

    }

void VarBddEvaluator::create_derived_var_bdds(int layer) {
  std::cout << "LAYER " << layer << "..." << std::flush;
  std::vector<int> rules_in_layer;
  // add all "unproblematic" axioms to var bdd
  for (size_t i = 0; i < g_axioms.size(); i++) {
    if (is_trivial(i)) {
      continue;
    }
    if (variable_stratification->get_layer(get_axiom_head(i)) == layer &&
        axiom_body_layer.at(i) < layer) {
      BDD body;
      get_body_bdd(i, body);
      var_bdds[get_axiom_head(i)] += body;
    }
  }
  // add vars of this layer to queue
  std::queue<int> open_vars;
  for (auto &var_bdd : var_bdds) {
    int var = var_bdd.first;
    if (variable_stratification->get_layer(var) == layer) {
      // std::cout << g_variable_name[var] << std::endl;
      open_vars.push(var);
    }
  }

  // Recursively evaluate var bdd
  while (!open_vars.empty()) {
    int var = open_vars.front();
    open_vars.pop();
    for (size_t i = 0; i < g_axioms.size(); i++) {
      if (is_trivial(i) || variable_stratification->get_layer(get_axiom_head(i)) != layer) {
        continue;
      }
      if (is_in_body(var, i)) {
        int head = get_axiom_head(i);

        /*if (axioms_layers[head] != layer) {
          g_axioms[i].dump();
        }*/
        assert(variable_stratification->get_layer(head) == layer);

        BDD res = var_bdds[head];
        BDD body;
        get_body_bdd(i, body);
        res += (body * var_bdds[var]);
        if (res != var_bdds[head]) {
          open_vars.push(head);
        }
        var_bdds[head] = res;
        // std::cout << g_variable_name[head] << " updated" << std::endl;
      }
    }
  }
  std::cout << "done!" << std::endl;
}

    void VarBddEvaluator::get_body_bdd(int axiom_id, BDD &res, int skip_var) const {
        res = vars->oneBDD();
        const GlobalEffect eff = g_axioms[axiom_id].get_effects()[0];
        for (auto pre : eff.conditions) {
            if (pre.var != skip_var) {
                if (variable_stratification->is_primary_variable(pre.var)) {
                    res *= vars->preBDD(pre.var, pre.val);
                } else {
                    if (pre.val == g_default_axiom_values[pre.var]) {
                        res *= !var_bdds.at(pre.var);
                    } else {
                        res *= var_bdds.at(pre.var);
                    }
                }
            }
        }
    }

    void VarBddEvaluator::create_derived_var_bdds() {
        create_axiom_body_layer();
        for (int i = 0; i < variable_stratification->num_variables(); i++) {
            if (variable_stratification->is_derived_variable(i)) {
                var_bdds[i] = vars->zeroBDD();
            }
        }

        // Call for each layer the recursive procedure
        for (int i = 0; i < variable_stratification->num_layers(); i++) {
            create_derived_var_bdds(i);
        }
    }

void VarBddEvaluator::init_axioms() {
  utils::Timer timer;
  create_abstract_cube();

  create_derived_var_bdds();

  for (auto &p : var_bdds) {
    // We need to invert the var_bdd if the default value is 1
    BDD def = vars->preBDD(p.first, 1 - g_default_axiom_values[p.first]);
    p.second = p.second.Xnor(def);
  }
  std::cout << std::fixed << "Symbolic Axiom initialization: " << timer
            << std::endl;
}

void VarBddEvaluator::apply_axioms(const BDD &bdd, BDD &res) const {
  utils::Timer timer;
  BDD result;
  abstract_derived(bdd, result);
  for (auto &p : var_bdds) {
    result *= p.second;
  }
  res = result;
  eval_time += timer.stop();
  std ::cout << std::fixed << "    => Axiom evaluation took " << timer
            << " [Overall: " << eval_time << "s"
            << "]" << std::endl;
}


    static shared_ptr<SymAxiomEvaluator> _parse_axiom_evaluator_transform(OptionParser &parser) {
        SymAxiomEvaluator::add_options_to_parser(parser);
        Options opts = parser.parse();
        if (parser.dry_run())
            return nullptr;
        else
            return make_shared<VarBddEvaluator>(opts);
    }


    static PluginShared<SymAxiomEvaluator> _plugin("varbdds", _parse_axiom_evaluator_transform);


}  // namespace symbolic
