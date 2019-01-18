#ifndef SEARCH_SYMBOLIC_SYM_AXIOM_STRATIFICATION_H_
#define SEARCH_SYMBOLIC_SYM_AXIOM_STRATIFICATION_H_

#include <map>
#include <set>
#include <vector>
#include "../../globals.h"
#include "../../option_parser_util.h"

namespace symbolic {

    class VariableStratification {

        int n_layers;
        std::vector<int> var_layers;
        std::vector<int> derived_vars;

    public:

    VariableStratification(std::vector<int> var_layers_) :
        n_layers(-1),var_layers(var_layers_) {
            for (size_t i = 0; i < var_layers.size(); ++i){
                if (var_layers[i] >= 0) {
                    derived_vars.push_back(i);
                    n_layers = std::max(n_layers, var_layers[i]);
                }
            }
            n_layers += 1;
            std::cout << "#Layers: " << n_layers << std::endl;
        }
        
        int num_variables() const {
            return var_layers.size();
        }

        bool is_derived_variable(int var) const {
            return var_layers[var] != -1;
        }

        int get_layer(int var) const {
            assert((size_t)var < var_layers.size());
            return var_layers[var];
        }

        bool is_primary_variable(int var) const {
            return  var_layers[var] == -1;
        }

        int num_layers() const {
            return n_layers;
        }

        const std::vector<int> & get_derived_vars() const {
            return derived_vars;
        }

    };  
    
    class AxiomStratification {
        std::vector<int> m_axiom_layer;
        std::vector<bool> m_is_fixpoint_layer; 

    public:
        /* AxiomStratification (int num_layers, const std::vector<int> & axiom_layer) : */
        /* m_is_fixpoint_layer(num_layers, true), m_axiom_layer (axiom_layer) { */
        /* } */

        AxiomStratification (const std::vector<int> & variable_layers);
        
        AxiomStratification (const std::vector<int> & axiom_layer,
                             const std::vector<bool> & fixpoint) :
        m_axiom_layer (axiom_layer), m_is_fixpoint_layer(fixpoint) {
            assert (m_axiom_layer.size() == g_axioms.size());
        }

        int get_num_layers() const {
            return m_is_fixpoint_layer.size();
        }

        bool is_fixpoint_layer(int layer) const {
            return m_is_fixpoint_layer[layer];
        }

        int get_layer(int axiom_id) const {
            assert((size_t)axiom_id < m_axiom_layer.size());
            return m_axiom_layer[axiom_id];
        }       
    };
    

    class Stratifier {
    public:
        Stratifier() {}
        virtual std::unique_ptr<VariableStratification> get_variable_stratification() const = 0;
        virtual std::unique_ptr<AxiomStratification> get_axiom_stratification() const = 0;
        virtual std::string get_name() const = 0;
    };

    class StratifierFD : public Stratifier {
    public:
    StratifierFD(const Options  & ) : Stratifier() {
        }
        virtual std::unique_ptr<VariableStratification> get_variable_stratification() const override;
        virtual std::unique_ptr<AxiomStratification> get_axiom_stratification() const override;

        virtual std::string get_name() const override {
            return "stratified_fd";
        }
    };
    
    class StratifierSCCVars : public Stratifier {
    public:
    StratifierSCCVars(const Options  & ) : Stratifier(){
        }

        virtual std::unique_ptr<VariableStratification> get_variable_stratification() const override {
            std::vector<int> var_layers;
            stratify_transitive_closure(var_layers);
            return std::make_unique<VariableStratification> (var_layers); 
            
        }
        virtual std::unique_ptr<AxiomStratification> get_axiom_stratification() const override;
        
        virtual std::string get_name() const override {
            return "stratified_scc";
        }
            

    protected:
        void order_derived(std::vector<std::vector<int>>& relation) const;
        void extract_derived(std::vector<std::vector<int>> const& realation,
                             std::vector<int>& var_layers) const;

        bool negatively_contained(int axiom_id, int var) const;
        bool positively_contained(int axiom_id, int var) const;

        void dependency_graph(const std::vector<std::set<int>>& scc_fixpoint,
                              const std::set<int>& scc_no_fixpoint,
                              std::map<int, std::set<int>>& dep) const;

        void free_verticies(const std::map<int, std::set<int>>& dep,
                            std::set<int>& vecticies) const;
        
        bool is_stratified(const std::vector<int>& var_layers) const;

        void stratify_transitive_closure(std::vector<int>& var_layers) const;

        void stratify_scc(std::vector<int>& axiom_layers,
                          std::vector<bool>& is_fixpoint) const;

    };
}  // namespace symbolic


// int SymAxiomEvaluator::get_axiom_layer(int axiom_id) {
//   return var_layers[get_axiom_head(axiom_id)];
// }



#endif /* SEARCH_SYMBOLIC_SYM_AXIOM_STRATIFICATION_H_ */
