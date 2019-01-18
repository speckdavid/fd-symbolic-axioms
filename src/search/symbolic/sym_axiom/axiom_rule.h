#ifndef SEARCH_SYMBOLIC_SYM_AXIOM_AXIOM_RULE_H_
#define SEARCH_SYMBOLIC_SYM_AXIOM_AXIOM_RULE_H_

#include "../sym_variables.h"
#include <string>

namespace symbolic {

// Similar to Transition Relations but axiom specific
class SymAxiomRule {
	SymVariables *sV; //To call basic BDD creation methods
	int layer; // Axiom layer
	BDD tBDD; // bdd for making the relprod
		
	std::vector<int> effVars; //FD Index of eff variables. Must be sorted!!
	BDD effectValue; //BDD representing the value 
	
	BDD existsVars, existsBwVars;   // Cube with variables to existentialize
	std::vector<BDD> swapVarsS, swapVarsSp; // Swap variables s to sp and vice versa
	std::vector<BDD> swapVarsA, swapVarsAp; // Swap abstraction variables

	std::set<const GlobalOperator *> axioms; //List of operators represented by the TR

public:
    //Constructor for axioms
    SymAxiomRule(SymVariables *sVars, const GlobalOperator *ax, int layer);

    //Copy constructor
    SymAxiomRule(const SymAxiomRule &) = default;

    BDD image(const BDD &from) const;
    BDD preimage(const BDD &from) const;
    BDD image(const BDD &from, int maxNodes) const;
    BDD preimage(const BDD &from, int maxNodes) const;

    BDD prepareTBDDToMerge() const;

    void merge(const SymAxiomRule &t2, int maxNodes);

    int getLayer() const {
        return layer;
    }

    void setLayer(int layer) {
        this->layer = layer;
    }

    int nodeCount() const {
        return tBDD.nodeCount();
    }

    const std::set<const GlobalOperator *> &getAxiom() const {
        return axioms;
    }

    const BDD &getBDD() const {
        return tBDD;
    }
    
    BDD get_remover() const {
    	return !(tBDD.AndAbstract(tBDD, existsBwVars));
    }

    const std::vector<int> & getEffectVars() const {
	return effVars;
    }

};

}



#endif /* SEARCH_SYMBOLIC_SYM_AXIOM_SYM_AXIOM_H */
