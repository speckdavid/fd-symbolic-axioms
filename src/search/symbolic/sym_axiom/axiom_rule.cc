#include "../../utils/debug_macros.h"
#include "../../utils/timer.h"
#include <algorithm>
#include "axiom_rule.h"

using namespace std;

namespace symbolic {

SymAxiomRule::SymAxiomRule(SymVariables *sVars, const GlobalOperator *ax, int layer) :
		sV(sVars), layer(layer), tBDD(sVars->oneBDD()), existsVars(
				sVars->oneBDD()), existsBwVars(sVars->oneBDD()) {
	axioms.insert(ax);
	for (size_t i = 0; i < ax->get_preconditions().size(); i++) { //Put precondition of label
		const GlobalCondition &prevail = ax->get_preconditions()[i];
		tBDD *= sV->preBDD(prevail.var, prevail.val);
	}
	// Only one effect with a real (not conditional) precondition
	// ax->dump();
	assert(ax->get_effects().size() == 1);
	const GlobalEffect &effect = ax->get_effects()[0];
	for (auto& cond : effect.conditions)
	{
	    tBDD *= sV->preBDD(cond.var, cond.val);
	}
	
	// Add the single effect
	int var = effect.var;
	effVars.push_back(var);
	effectValue = sV->preBDD(var, effect.val);

	assert(!tBDD.IsZero());

	// Set swap variable (multiple because of binary representations)
	for (int var : effVars) {
		for (int bdd_var : sV->vars_index_pre(var)) {
			swapVarsS.push_back(sV->bddVar(bdd_var));
		}
		for (int bdd_var : sV->vars_index_eff(var)) {
			swapVarsSp.push_back(sV->bddVar(bdd_var));
		}
	}
	assert(swapVarsS.size() == swapVarsSp.size());

	// existsVars/existsBwVars is just the conjunction of swapVarsS and swapVarsSp
	for (size_t i = 0; i < swapVarsS.size(); ++i) {
		existsVars *= swapVarsS[i];
		existsBwVars *= swapVarsSp[i];
	}
}

    BDD SymAxiomRule::image(const BDD &from) const {
	BDD tmp = tBDD.AndAbstract(from, existsVars);
	if (effVars.size() == 1) {
	    return tmp*effectValue + (!tBDD)*from ;
	} else {
	    return tmp.SwapVariables(swapVarsS, swapVarsSp);
	}
    }

    BDD SymAxiomRule::image(const BDD &from, int maxNodes) const {
	BDD tmp = tBDD.AndAbstract(from, existsVars, maxNodes);
	if (effVars.size() == 1) {
	    return tmp*effectValue + (!tBDD)*from ;
	} else {
	    return tmp.SwapVariables(swapVarsS, swapVarsSp);
	}
    }

BDD SymAxiomRule::preimage(const BDD &from) const {
    if (effVars.size() == 1) {
	cout << "Unsupported preimage on axiom rule" << endl;
	utils::exit_with(utils::ExitCode::UNSUPPORTED);
    } else {
	BDD tmp = from.SwapVariables(swapVarsS, swapVarsSp);
	BDD res = tBDD.AndAbstract(tmp, existsBwVars);
	return res;
    }
}

BDD SymAxiomRule::prepareTBDDToMerge() const {
    if (effVars.size() == 1){
	BDD effectYes = effectValue.SwapVariables(swapVarsS, swapVarsSp);
	BDD effectNo = !effectYes;	
	return ((tBDD + effectValue)*effectYes) + ((!tBDD)*!effectValue*effectNo);
    }
    
    return tBDD;
}

BDD SymAxiomRule::preimage(const BDD &from, int maxNodes) const {
    if (effVars.size() == 1) {
	cout << "Unsupported preimage on axiom rule" << endl;
	utils::exit_with(utils::ExitCode::UNSUPPORTED);
    } else {
	utils::Timer t;
	DEBUG_MSG(cout << "Image layer " << layer << " from " << from.nodeCount() << " with " << tBDD.nodeCount() << flush;);
	BDD tmp = from.SwapVariables(swapVarsS, swapVarsSp);
	DEBUG_MSG(cout << " tmp " << tmp.nodeCount() << " in " << t() << flush;);
	BDD res = tBDD.AndAbstract(tmp, existsBwVars, maxNodes);
	DEBUG_MSG(cout << "res " << res.nodeCount() << " took " << t(););
	DEBUG_MSG(cout << endl;);
	return res;
    }
}

void SymAxiomRule::merge(const SymAxiomRule &t2,
                          int maxNodes) {
    assert(layer == t2.getLayer());
    if (layer != t2.getLayer()) {
        cout << "Error: merging transitions with different layer: " << layer << " " << t2.getLayer() << endl;
        utils::exit_with(utils::ExitCode::CRITICAL_ERROR);
    }

    //Special case of merging two axiom rules with the same head: we just compute the OR of the bodies
    if (effVars.size() == 1 && t2.effVars.size() == 1 && effVars[0] == t2.effVars[0]) {
	assert (effectValue == t2.effectValue);
	tBDD += t2.tBDD;
	return; 
    }  
 
    //  cout << "set_union" << endl;
    //Attempt to generate the new tBDD
    vector <int> newEffVars;
    set_union(effVars.begin(), effVars.end(),
              t2.effVars.begin(), t2.effVars.end(),
              back_inserter(newEffVars));

    // Assertion: avoid merging any TRs with more than one effect over the same variable
    if (newEffVars.size() != effVars.size() + t2.effVars.size()) {
	cout << "Error:  avoid merging any TRs with more than one effect over the same variable" << endl;
	utils::exit_with(utils::ExitCode::CRITICAL_ERROR);
    }
    
    // //    cout << "Eff vars" << endl;
    // vector<int>::const_iterator var1 = effVars.begin();
    // vector<int>::const_iterator var2 = t2.effVars.begin();
    // for (vector<int>::const_iterator var = newEffVars.begin();
    //      var != newEffVars.end(); ++var) {
    //     if (var1 == effVars.end() || *var1 != *var) {
    //         newTBDD *= sV->biimp(*var);
    //     } else {
    //         ++var1;
    //     }

    //     if (var2 == t2.effVars.end() || *var2 != *var) {
    //         newTBDD2 *= sV->biimp(*var);
    //     } else {
    //         ++var2;
    //     }
    // }

    BDD newTBDD = prepareTBDDToMerge();
    BDD newTBDD2 = t2.prepareTBDDToMerge();

    newTBDD = newTBDD.And(newTBDD2, maxNodes);

    if (newTBDD.nodeCount() > maxNodes) {
        DEBUG_MSG(cout << "TR size exceeded: " << newTBDD.nodeCount() << ">" << maxNodes << endl;);
        throw BDDError(); //We could not sucessfully merge
    }

    tBDD = newTBDD;

    effVars.swap(newEffVars);
    existsVars *= t2.existsVars;
    existsBwVars *= t2.existsBwVars;

    for (size_t i = 0; i < t2.swapVarsS.size(); i++) {
        if (find(swapVarsS.begin(), swapVarsS.end(), t2.swapVarsS[i]) ==
            swapVarsS.end()) {
            swapVarsS.push_back(t2.swapVarsS[i]);
            swapVarsSp.push_back(t2.swapVarsSp[i]);
        }
    }

    axioms.insert(t2.axioms.begin(), t2.axioms.end());
}

}

