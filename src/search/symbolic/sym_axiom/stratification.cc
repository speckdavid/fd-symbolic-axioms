#include "stratification.h"
#include <algorithm>
#include <list>
#include <stack>
#include "../../utils/debug_macros.h"
#include "../../utils/system.h"
#include "axiom_evaluator.h"
#include "../../plugin.h"
#include "../option_parser.h"
#include "../../utils/sccs.h"

namespace symbolic {
    using namespace std;
    bool StratifierSCCVars::is_stratified(const std::vector<int>& var_layers) const {
        for (size_t i = 0; i < g_axioms.size(); i++) {
            if (SymAxiomEvaluator::is_trivial(i)) {
                continue;
            }
            int head_layer = var_layers[g_axioms[i].get_effects()[0].var];
            auto body = g_axioms[i].get_effects()[0].conditions;
            for (auto& cond : body) {
                int cond_layer = var_layers[cond.var];
                if (g_default_axiom_values[cond.var] == -1) {
                    continue;
                }
                if (cond.val != g_default_axiom_values[cond.var]) {
                    if (cond_layer > head_layer) {
                        return false;
                    }
                } else {
                    if (cond_layer >= head_layer) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

/******************** Stratification from "In defense of PDDL axioms"
 * *********************************/
    void StratifierSCCVars::stratify_transitive_closure(
        std::vector<int>& var_layers) const {
        var_layers = std::vector<int>(g_variable_name.size(), -1);
        std::vector<std::vector<int>> relation;
        order_derived(relation);
        for (size_t i = 0; i < g_variable_name.size(); i++) {
            if (relation[i][i] == 2) {
                std::cout << "FAIL => not possible to stratify!" << std::endl;
                utils::exit_with(utils::ExitCode::CRITICAL_ERROR);
            }
        }
        extract_derived(relation, var_layers);
    }

    void StratifierSCCVars::order_derived(std::vector<std::vector<int>>& relation) const {
        for (size_t i = 0; i < g_variable_name.size(); i++) {
            relation.push_back(std::vector<int>(g_variable_name.size(), 0));
        }

        for (int axiom_id = 0; axiom_id < (int)g_axioms.size(); axiom_id++) {
            if (SymAxiomEvaluator::is_trivial(axiom_id)) {
                continue;
            }
            int head = SymAxiomEvaluator::get_axiom_head(axiom_id);
            for (int var = 0; var < static_cast<int>(g_variable_name.size()); var++) {
                if (g_axiom_layers[var] == -1) {
                    continue;
                }
                if (negatively_contained(axiom_id, var)) {
                    relation[var][head] = 2;
                } else if (positively_contained(axiom_id, var)) {
                    relation[var][head] = std::max(1, relation[var][head]);
                }
            }
        }
        for (int j = 0; j < static_cast<int>(g_variable_name.size()); j++) {
            if (g_axiom_layers[j] == -1) {
                continue;
            }
            for (int i = 0; i < static_cast<int>(g_variable_name.size()); i++) {
                if (g_axiom_layers[i] == -1) {
                    continue;
                }
                for (int k = 0; k < static_cast<int>(g_variable_name.size()); k++) {
                    if (g_axiom_layers[k] == -1) {
                        continue;
                    }
                    if (std::min(relation[i][j], relation[j][k]) > 0) {
                        relation[i][k] = std::max(std::max(relation[i][j], relation[j][k]),
                                                  relation[i][k]);
                    }
                }
            }
        }
        for (int i = 0; i < static_cast<int>(g_variable_name.size()); i++) {
            if (g_axiom_layers[i] == -1) {
                continue;
            }
            for (int j = 0; j < static_cast<int>(g_variable_name.size()); j++) {
                if (g_axiom_layers[j] == -1) {
                    continue;
                }
            }
        }
    }

    void StratifierSCCVars::extract_derived(std::vector<std::vector<int>> const& relation,
                                            std::vector<int>& var_layers) const {
        std::set<int> stratification;
        std::set<int> remaining;
        for (int i = 0; i < static_cast<int>(g_variable_name.size()); i++) {
            if (g_axiom_layers[i] == -1) {
                continue;
            }
            remaining.insert(i);
        }
        int level = 0;
        while (!remaining.empty()) {
            std::set<int> stratum;
            for (int j : remaining) {
                bool all = true;
                for (int i : remaining) {
                    if (relation[i][j] == 2) {
                        all = false;
                        break;
                    }
                }
                if (all) {
                    stratum.insert(j);
                }
            }
            std::set<int> new_remaining;
            std::set_difference(remaining.begin(), remaining.end(), stratum.begin(),
                                stratum.end(),
                                std::inserter(new_remaining, new_remaining.begin()));
            remaining = new_remaining;
            for (int i : stratum) {
                var_layers[i] = level;
            }
            level++;
        }
    }

    bool StratifierSCCVars::negatively_contained(int axiom_id, int var) const {
        for (auto& pre : g_axioms[axiom_id].get_effects()[0].conditions) {
            if (pre.var == var && g_default_axiom_values[pre.var] == pre.val) {
                return true;
            }
        }
        return false;
    }

    bool StratifierSCCVars::positively_contained(int axiom_id, int var) const {
        for (auto& pre : g_axioms[axiom_id].get_effects()[0].conditions) {
            if (pre.var == var && g_default_axiom_values[pre.var] != pre.val) {
                return true;
            }
        }
        return false;
    }

/******************** Stratification based on SCC ***************************/

    class Graph {
        int V;                // No. of vertices
        std::list<int>* adj;  // An array of adjacency lists

        // Fills Stack with vertices (in increasing order of finishing times). The top
        // element of stack has the maximum finishing time
        void fillOrder(int v, bool visited[], std::stack<int>& Stack);

        // A recursive function to print DFS starting from v
        void DFSUtil(int v, bool visited[], std::vector<std::vector<int>>& scc,
                     bool print = false);

    public:
        Graph(int V);
        void addEdge(int v, int w);

        // The main function that finds and prints strongly connected
        // components
        void computeSCCs(std::vector<std::vector<int>>& scc, bool print = false);

        // Function that returns reverse (or transpose) of this graph
        Graph getTranspose();
    };

    Graph::Graph(int V) {
        this->V = V;
        adj = new std::list<int>[V];
    }

// A recursive function to print DFS starting from v
    void Graph::DFSUtil(int v, bool visited[], std::vector<std::vector<int>>& scc,
                        bool print) {
        // Mark the current node as visited and print it
        visited[v] = true;
        if (print) std::cout << v << " ";
        scc.back().push_back(v);

        // Recur for all the vertices adjacent to this vertex
        std::list<int>::iterator i;
        for (i = adj[v].begin(); i != adj[v].end(); ++i)
            if (!visited[*i]) DFSUtil(*i, visited, scc, print);
    }

    Graph Graph::getTranspose() {
        Graph g(V);
        for (int v = 0; v < V; v++) {
            // Recur for all the vertices adjacent to this vertex
            std::list<int>::iterator i;
            for (i = adj[v].begin(); i != adj[v].end(); ++i) {
                g.adj[*i].push_back(v);
            }
        }
        return g;
    }

    void Graph::addEdge(int v, int w) {
        adj[v].push_back(w);  // Add w to v’s list.
    }

    void Graph::fillOrder(int v, bool visited[], std::stack<int>& Stack) {
        // Mark the current node as visited and print it
        visited[v] = true;

        // Recur for all the vertices adjacent to this vertex
        std::list<int>::iterator i;
        for (i = adj[v].begin(); i != adj[v].end(); ++i)
            if (!visited[*i]) fillOrder(*i, visited, Stack);

        // All vertices reachable from v are processed by now, push v
        Stack.push(v);
    }

    // The main function that finds and prints all strongly connected components
    void Graph::computeSCCs(std::vector<std::vector<int>>& scc, bool print) {
        std::stack<int> Stack;

        // Mark all the vertices as not visited (For first DFS)
        bool* visited = new bool[V];
        for (int i = 0; i < V; i++) visited[i] = false;

        // Fill vertices in stack according to their finishing times
        for (int i = 0; i < V; i++)
            if (visited[i] == false) fillOrder(i, visited, Stack);

        // Create a reversed graph
        Graph gr = getTranspose();

        // Mark all the vertices as not visited (For second DFS)
        for (int i = 0; i < V; i++) visited[i] = false;

        // Now process all vertices in order defined by Stack
        while (Stack.empty() == false) {
            // Pop a vertex from stack
            int v = Stack.top();
            Stack.pop();

            // Print Strongly connected component of the popped vertex
            if (visited[v] == false) {
                scc.push_back(std::vector<int>());
                gr.DFSUtil(v, visited, scc, print);
                if (print) std::cout << std::endl;
            }
        }
    }

    void StratifierSCCVars::dependency_graph(
        const std::vector<std::set<int>>& scc_fixpoint,
        const std::set<int>& scc_no_fixpoint,
        std::map<int, std::set<int>>& dep) const {
        assert(dep.empty());
        for (int ax : scc_no_fixpoint) {
            dep[ax] = std::set<int>();
        }
        for (size_t s = 0; s < scc_fixpoint.size(); s++) {
            dep[-(s + 1)] = std::set<int>();
        }
        for (size_t i = 0; i < g_axioms.size(); i++) {
            for (size_t j = 0; j < g_axioms.size(); j++) {
                int head = SymAxiomEvaluator::get_axiom_head(i);

                if (SymAxiomEvaluator::is_in_body(head, j)) {
                    int first = i;
                    for (size_t s = 0; s < scc_fixpoint.size(); s++) {
                        if (scc_fixpoint.at(s).count(first)) {
                            first = -(s + 1);
                            break;
                        }
                    }
                    int second = j;
                    for (size_t s = 0; s < scc_fixpoint.size(); s++) {
                        if (scc_fixpoint.at(s).count(second)) {
                            second = -(s + 1);
                            break;
                        }
                    }
                    if (first == second) {
                        continue;
                    }

                    // second depends on first => second has an incoming edge
                    dep[second].insert(first);
                }
            }
        }
    }

    void StratifierSCCVars::free_verticies(const std::map<int, std::set<int>>& dep,
                                           std::set<int>& vecticies) const {
        assert(vecticies.empty());
        for (auto& entry : dep) {
            if (entry.second.empty()) {
                vecticies.insert(entry.first);
            }
        }
    }

// Mapping from axiom => level and axiom => fixpoint?
    void StratifierSCCVars::stratify_scc(std::vector<int>& axiom_layers,
                                         std::vector<bool>& fixpoint) const {
        std::cout << "SCC Stratification: " << std::endl;
        assert(axiom_layers.size() == 0);
        assert(fixpoint.size() == 0);
        axiom_layers = std::vector<int>(g_axioms.size(), -1);

        // Graph g(g_axioms.size());
        // for (size_t i = 0; i < g_axioms.size(); i++) {
        //     for (size_t j = 0; j < g_axioms.size(); j++) {
        //         int head = SymAxiomEvaluator::get_axiom_head(i);
        //         if (SymAxiomEvaluator::is_in_body(head, j)) {
        //             g.addEdge(i, j);
        //         }
        //     }
        // }
        // std::vector<std::vector<int>> scc;
        // g.computeSCCs(scc, false);

        std::vector<std::vector<int>> graph (g_axioms.size());
        std::vector<std::vector<int>> var_to_axiom (g_variable_domain.size());
        for (size_t i = 0; i < g_axioms.size(); i++) {
            for (auto &pre : g_axioms[i].get_effects()[0].conditions) {
                var_to_axiom[pre.var].push_back(i);
            }            
        }
        
        for (size_t i = 0; i < g_axioms.size(); i++) {
            for (int j : var_to_axiom[SymAxiomEvaluator::get_axiom_head(i)]) {
                graph[i].push_back(j);
            }
        }

        std::vector<std::vector<int>> scc = utils::compute_maximal_sccs(graph);
        std::vector<std::set<int>> scc_fixpoint;
        std::set<int> scc_no_fixpoint;

        cout << scc.size() << "SCCs" <<  endl;
        std::vector<int> axiom_to_scc (g_axioms.size());
        for (size_t i = 0; i < scc.size(); i++) {
            for (int ax : scc[i]){
                axiom_to_scc[ax] = i;
            }
        }

        std::vector<std::set<int>> scc_graph (scc.size());
        std::vector<int> scc_pre_count(scc.size(), 0);
        for (size_t i = 0; i < scc.size(); i++) {
            for (int ax : scc[i]) {
                for(int ax2 : graph[ax]) {
                    int scc2 = axiom_to_scc[ax2];
                    if (scc2 == (int)i) continue;

                    if(!scc_graph[i].count(scc2)) {
                        scc_graph[i].insert(scc2);
                        scc_pre_count[scc2] ++;
                        assert(!scc_graph[scc2].count(i));
                    }
                }
            }
        }

        int layer = 0;
        std::vector<int> ready_sccs_no_fixpoint, ready_sccs_fixpoint;
        for (size_t i = 0; i < scc.size(); i++) {
            if (scc_pre_count[i] == 0){
                if(scc[i].size() == 1) {
                    ready_sccs_no_fixpoint.push_back(i);
                } else {
                    ready_sccs_fixpoint.push_back(i);
                }
            }
        }

        size_t num_included = 0;

        while (!ready_sccs_no_fixpoint.empty() ||  !ready_sccs_fixpoint.empty()) {
            vector<int> processed_sccs; 
            if(!ready_sccs_fixpoint.empty()) {

                int scc_index = ready_sccs_fixpoint.back();
                ready_sccs_fixpoint.pop_back();
                fixpoint.push_back(true);
                for(int ax : scc[scc_index]) {
                    num_included++;
                    axiom_layers[ax] = layer;                   
                }
                processed_sccs.push_back(scc_index);
            } else {

                fixpoint.push_back(false);
                for (int scc_index : ready_sccs_no_fixpoint) {
                    processed_sccs.push_back(scc_index);
                    assert(scc[scc_index].size() == 1);
                    for(int ax : scc[scc_index]) {
                        num_included++;
                        axiom_layers[ax] = layer;                   
                    }
                }
                processed_sccs.swap(ready_sccs_no_fixpoint);
                ready_sccs_no_fixpoint.clear();
            }
            
            for(int scc_index : processed_sccs) {
                assert(scc_pre_count[scc_index]-- == 0);
                for(int scc_enabled : scc_graph[scc_index]) {
                    if(--scc_pre_count[scc_enabled] == 0) {
                        if(scc[scc_enabled].size() == 1) {
                            ready_sccs_no_fixpoint.push_back(scc_enabled);
                        } else {
                            ready_sccs_fixpoint.push_back(scc_enabled);
                        }
                    }
                }
            }

            layer++;
        }
        
        assert(num_included == g_axioms.size());


        // for (size_t i = 0; i < scc.size(); i++) {
        //     if (scc[i].size() == 1) {
        //         scc_no_fixpoint.insert(scc[i][0]);
        //     } else {
        //         scc_fixpoint.push_back(std::set<int>(scc[i].begin(), scc[i].end()));
        //     }
        // }

        // std::cout << " - #SCC: " << scc_no_fixpoint.size() << " " << scc_fixpoint.size() << std::endl;

        // /// Process non-fixpoint axioms (-n stands for n-th scc)
        // std::map<int, std::set<int>> dep;
        // dependency_graph(scc_fixpoint, scc_no_fixpoint, dep);

        // cout << "Dependency graph built" << endl;
        // /// Layer everything
        // int layer = 0;
        // while (dep.size()) {
        //     std::set<int> cur_axioms;
        //     free_verticies(dep, cur_axioms);

        //     // There exists only fixpoint layer
        //     if (*std::max_element(cur_axioms.begin(), cur_axioms.end()) < 0) {
        //         layer--;
        //     } else {
        //         DEBUG_MSG(std::cout << " - Layer " << layer << " => No Fixpoint."
        //                   << std::endl;);
        //         fixpoint.push_back(false);
        //     }

        //     // Collect current axioms without a dependency
        //     while (!cur_axioms.empty()) {
        //         int axiom_id = *std::max_element(cur_axioms.begin(), cur_axioms.end());
        //         // std::cout << axiom_id << std::endl;
        //         if (axiom_id < 0) {
        //             layer++;
        //             DEBUG_MSG(std::cout << " - Layer " << layer << " => Fixpoint."
        //                       << std::endl;);
        //             fixpoint.push_back(true);
        //             for (int ax : scc_fixpoint[-(axiom_id + 1)]) {
        //                 axiom_layers[ax] = layer;
        //             }
        //         } else {
        //             axiom_layers[axiom_id] = layer;
        //         }

        //         for (auto& entry : dep) {
        //             entry.second.erase(axiom_id);
        //         }

        //         dep.erase(dep.find(axiom_id));
        //         cur_axioms.erase(axiom_id);
        //     }
        //     // std::cout << std::endl;
        //     layer++;
        // }

        assert((*std::min_element(axiom_layers.begin(), axiom_layers.end())) == 0);
        assert((*std::max_element(axiom_layers.begin(), axiom_layers.end())) ==
               layer - 1);
        assert(static_cast<int>(fixpoint.size()) == layer);
        std::cout << " - #Layers: " << layer << std::endl;
    }


    AxiomStratification::AxiomStratification (const std::vector<int> & variable_layers) :
        m_axiom_layer(g_axioms.size()), m_is_fixpoint_layer(g_axioms.size(), true) {
            for (size_t i = 0; i < g_axioms.size(); i++) {
                m_axiom_layer[i] = variable_layers[SymAxiomEvaluator::get_axiom_head(i)];
            }

            assert (m_axiom_layer.size() == g_axioms.size());
        }


    std::unique_ptr<AxiomStratification> StratifierSCCVars::get_axiom_stratification() const {
        std::vector<int> axiom_layers;
        std::vector<bool> fixpoint;
        stratify_scc(axiom_layers, fixpoint);
        return make_unique<AxiomStratification> (axiom_layers, fixpoint);
    }


    static shared_ptr<Stratifier> _parse_stratifier_scc_vars(OptionParser &parser) {
        Options opts = parser.parse();
        if (parser.dry_run())
            return nullptr;
        else
            return make_shared<StratifierSCCVars>(opts);
    }


    static PluginShared<Stratifier> _plugin("scc", _parse_stratifier_scc_vars);


    std::unique_ptr<AxiomStratification> StratifierFD::get_axiom_stratification() const {
        return make_unique<AxiomStratification>(g_axiom_layers);
    }

    std::unique_ptr<VariableStratification> StratifierFD::get_variable_stratification() const {
        return make_unique<VariableStratification>(g_axiom_layers);
    }

    
    static shared_ptr<Stratifier> _parse_stratifier_fd(OptionParser &parser) {
        Options opts = parser.parse();
        if (parser.dry_run())
            return nullptr;
        else
            return make_shared<StratifierFD>(opts);
    }


    static PluginShared<Stratifier> _plugin_fd("default", _parse_stratifier_fd);


    static PluginTypePlugin<Stratifier> _type_plugin(
        "Stratifier",
        "stratifier");


}  // namespace symbolic
