#include "../../utils/debug_macros.h"
#include "../../utils/timer.h"
#include <algorithm>
#include "axiom_tr.h"

using namespace std;

namespace symbolic {

SymAxiom::SymAxiom(SymVariables *sVars, const GlobalOperator *ax, int layer) :
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
	for (auto & cond : effect.conditions)
	{
		tBDD *= sV->preBDD(cond.var, cond.val);
	}

	// Only need precondition of the axiom with an empty effect
	// Add the single effect
	int var = effect.var;
	effVars.push_back(var);
	tBDD *= sV->effBDD(var, effect.val);

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

BDD SymAxiom::image(const BDD &from) const {
    BDD aux = from;
    if (!swapVarsA.empty()) {
        aux = from.SwapVariables(swapVarsA, swapVarsAp);
    }
    BDD tmp = tBDD.AndAbstract(aux, existsVars);
    BDD res = tmp.SwapVariables(swapVarsS, swapVarsSp);
    return res;
}

BDD SymAxiom::image(const BDD &from, int maxNodes) const {
    DEBUG_MSG(cout << "Image layer " << layer << " from " << from.nodeCount() << " with " << tBDD.nodeCount(););
    BDD aux = from;
    if (!swapVarsA.empty()) {
        aux = from.SwapVariables(swapVarsA, swapVarsAp);
    }
    utils::Timer t;
    BDD tmp = tBDD.AndAbstract(aux, existsVars, maxNodes);
    DEBUG_MSG(cout << " tmp " << tmp.nodeCount() << " in " << t(););
    BDD res = tmp.SwapVariables(swapVarsS, swapVarsSp);
    DEBUG_MSG(cout << " res " << res.nodeCount() << " took " << t(););
    DEBUG_MSG(cout << endl;);

    return res;
}

BDD SymAxiom::preimage(const BDD &from) const {
    BDD tmp = from.SwapVariables(swapVarsS, swapVarsSp);
    BDD res = tBDD.AndAbstract(tmp, existsBwVars);
    if (!swapVarsA.empty()) {
        res = res.SwapVariables(swapVarsA, swapVarsAp);
    }
    return res;
}

BDD SymAxiom::preimage(const BDD &from, int maxNodes) const {
    utils::Timer t;
    DEBUG_MSG(cout << "Image layer " << layer << " from " << from.nodeCount() << " with " << tBDD.nodeCount() << flush;);
    BDD tmp = from.SwapVariables(swapVarsS, swapVarsSp);
    DEBUG_MSG(cout << " tmp " << tmp.nodeCount() << " in " << t() << flush;);
    BDD res = tBDD.AndAbstract(tmp, existsBwVars, maxNodes);
    if (!swapVarsA.empty()) {
        res = res.SwapVariables(swapVarsA, swapVarsAp);
    }
    DEBUG_MSG(cout << "res " << res.nodeCount() << " took " << t(););
    DEBUG_MSG(cout << endl;);

    return res;
}

void SymAxiom::merge(const SymAxiom &t2,
                          int maxNodes) {
    assert(layer == t2.getLayer());
    if (layer != t2.getLayer()) {
        cout << "Error: merging transitions with different layer: " << layer << " " << t2.getLayer() << endl;
        utils::exit_with(utils::ExitCode::CRITICAL_ERROR);
    }

    //  cout << "set_union" << endl;
    //Attempt to generate the new tBDD
    vector <int> newEffVars;
    set_union(effVars.begin(), effVars.end(),
              t2.effVars.begin(), t2.effVars.end(),
              back_inserter(newEffVars));

    BDD newTBDD = tBDD;
    BDD newTBDD2 = t2.tBDD;

    //    cout << "Eff vars" << endl;
    vector<int>::const_iterator var1 = effVars.begin();
    vector<int>::const_iterator var2 = t2.effVars.begin();
    for (vector<int>::const_iterator var = newEffVars.begin();
         var != newEffVars.end(); ++var) {
        if (var1 == effVars.end() || *var1 != *var) {
            newTBDD *= sV->biimp(*var);
        } else {
            ++var1;
        }

        if (var2 == t2.effVars.end() || *var2 != *var) {
            newTBDD2 *= sV->biimp(*var);
        } else {
            ++var2;
        }
    }
    newTBDD = newTBDD.Or(newTBDD2, maxNodes);

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

