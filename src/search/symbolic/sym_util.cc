#include "sym_util.h"


namespace symbolic {
TransitionRelation mergeTR(TransitionRelation tr, const TransitionRelation &tr2, int maxSize) {
    tr.merge(tr2, maxSize);
    return tr;
}
SymAxiom mergeAxiom(SymAxiom ax, const SymAxiom &ax2, int maxSize) {
    ax.merge(ax2, maxSize);
    return ax;
}

    SymAxiomRule mergeAxiomRule(SymAxiomRule ax, const SymAxiomRule &ax2, int maxSize) {
    ax.merge(ax2, maxSize);
    return ax;
}

BDD mergeAndBDD(const BDD &bdd, const BDD &bdd2, int maxSize) {
    return bdd.And(bdd2, maxSize);
}
BDD mergeOrBDD(const BDD &bdd, const BDD &bdd2, int maxSize) {
    return bdd.Or(bdd2, maxSize);
}
ADD mergeSumADD(const ADD &add, const ADD &add2, int ) {
    return add + add2;
}

ADD mergeMaxADD(const ADD &add, const ADD &add2, int ) {
    return add.Maximum(add2);
}
}
