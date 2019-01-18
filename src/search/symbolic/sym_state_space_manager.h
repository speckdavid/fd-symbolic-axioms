#ifndef SYMBOLIC_SYM_STATE_SPACE_MANAGER_H
#define SYMBOLIC_SYM_STATE_SPACE_MANAGER_H

#include "../operator_cost_function.h"
#include "../utils/system.h"
#include "sym_bucket.h"
#include "sym_enums.h"
#include "sym_util.h"
#include "sym_variables.h"

#include <cassert>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "sym_axiom/axiom_evaluator.h"

namespace options {
class OptionParser;
class Options;
}  // namespace options

namespace symbolic {
class SymVariables;
class TransitionRelation;
class Stratifier;
class AxiomEvaluator;

/*
 * All the methods may throw exceptions in case the time or nodes are exceeded.
 *
 */
class SymParamsMgr {
 public:
  // Parameters to generate the TRs
  int max_tr_size, max_tr_time;

  // Parameters to generate the mutex BDDs
  MutexType mutex_type;
  int max_mutex_size, max_mutex_time;

  // Axiom handling
  std::shared_ptr<SymAxiomEvaluator> axiom_evaluator;

  // Time and memory bounds for auxiliary operations
  int max_aux_nodes, max_aux_time;

  SymParamsMgr();
  SymParamsMgr(const options::Options &opts);
  static void add_options_to_parser(options::OptionParser &parser);
  void print_options() const;
};

class SymStateSpaceManager {
  void zero_preimage(const BDD &bdd, std::vector<BDD> &res, int maxNodes) const;
  void cost_preimage(const BDD &bdd, std::map<int, std::vector<BDD>> &res,
                     int maxNodes) const;
  void zero_image(const BDD &bdd, std::vector<BDD> &res, int maxNodes) const;
  void cost_image(const BDD &bdd, std::map<int, std::vector<BDD>> &res,
                  int maxNodes) const;

 protected:
  SymVariables *vars;
  const SymParamsMgr p;

  // If the variable is fully/partially/not considered in the abstraction
  std::set<int> relevant_vars;

  BDD initialState;  // initial state
  BDD goal;  // bdd representing the true (i.e. not simplified) goal-state

  std::map<int, std::vector<TransitionRelation>> transitions;  // TRs
  int min_transition_cost;  // minimum cost of non-zero cost transitions
  bool hasTR0;              // If there is transitions with cost 0

  std::shared_ptr<SymAxiomEvaluator> axiomEvaluator;  // Used to apply axioms

  // BDD representation of valid states (wrt mutex) for fw and bw search
  std::vector<BDD> notMutexBDDsFw, notMutexBDDsBw;

  // Dead ends for fw and bw searches. They are always removed in
  // filter_mutex (it does not matter which mutex_type we are using).
  std::vector<BDD> notDeadEndFw, notDeadEndBw;

  BDD getRelVarsCubePre() const { return vars->getCubePre(relevant_vars); }

  BDD getRelVarsCubeEff() const { return vars->getCubeEff(relevant_vars); }

  virtual std::string tag() const = 0;

  void init_transitions(
      const std::map<int, std::vector<TransitionRelation>> &(indTRs));
  bool is_relevant_op(const GlobalOperator &op) const;

 public:
  SymStateSpaceManager(SymVariables *v, const SymParamsMgr &params,
                       const std::set<int> &relevant_vars_ = std::set<int>());

  inline bool isAbstracted() const { return !isOriginal(); }

  inline bool isOriginal() const {
    return relevant_vars.size() == g_variable_domain.size();
  }

  virtual BDD shrinkExists(const BDD &bdd, int maxNodes) const = 0;
  virtual BDD shrinkForall(const BDD &bdd, int maxNodes) const = 0;
  virtual BDD shrinkTBDD(const BDD &bdd, int maxNodes) const = 0;

  void filterMutex(Bucket &bucket, bool fw, bool initialization);
  void mergeBucket(Bucket &bucket) const;
  void mergeBucketAnd(Bucket &bucket) const;

  void shrinkBucket(Bucket &bucket, int maxNodes);

  void addDeadEndStates(bool fw, BDD bdd);

  void addDeadEndStates(const std::vector<BDD> &fw_dead_ends,
                        const std::vector<BDD> &bw_dead_ends);

  inline SymVariables *getVars() const { return vars; }

  inline const SymParamsMgr getParams() const { return p; }

  inline const std::set<int> &get_relevant_variables() const {
    return relevant_vars;
  }

  inline bool isRelevantVar(int var) const {
    return relevant_vars.count(var) > 0;
  }

  int numVariablesToAbstract() const { return relevant_vars.size(); }

  int numVariablesAbstracted() const {
    return g_variable_domain.size() - relevant_vars.size();
  }

  double stateCount(const Bucket &bucket) const {
    return vars->numStates(bucket);
  }

  inline BDD shrinkForall(const BDD &bdd) {
    setTimeLimit(p.max_aux_time);
    try {
      BDD res = shrinkForall(bdd, p.max_aux_nodes);
      unsetTimeLimit();
      return res;
    } catch (BDDError e) {
      unsetTimeLimit();
    }
    return zeroBDD();
  }

  inline long totalNodes() const { return vars->totalNodes(); }

  inline unsigned long totalMemory() const { return vars->totalMemory(); }

  inline const BDD &getGoal() { return goal; }

  inline const BDD &getInitialState() { return initialState; }

  // Update binState
  inline int *getBinaryDescription(const GlobalState &state) const {
    return vars->getBinaryDescription(state);
  }

  inline BDD getBDD(int variable, int value) const {
    return vars->preBDD(variable, value);
  }

  inline Cudd *mgr() const { return vars->mgr(); }

  inline BDD zeroBDD() const { return vars->zeroBDD(); }

  inline BDD oneBDD() const { return vars->oneBDD(); }

  inline const std::vector<BDD> &getNotMutexBDDs(bool fw) const {
    return fw ? notMutexBDDsFw : notMutexBDDsBw;
  }

  inline const std::vector<BDD> &getNotDeadEnds(bool fw) const {
    return fw ? notDeadEndFw : notDeadEndBw;
  }

  bool mergeBucket(Bucket &bucket, int maxTime, int maxNodes) const {
    auto mergeBDDs = [](BDD bdd, BDD bdd2, int maxNodes) {
      return bdd.Or(bdd2, maxNodes);
    };
    merge(vars, bucket, mergeBDDs, maxTime, maxNodes);
    removeZero(bucket);  // Be sure that we do not contain only the zero BDD

    return bucket.size() <= 1;
  }

  bool mergeBucketAnd(Bucket &bucket, int maxTime, int maxNodes) const {
    auto mergeBDDs = [](BDD bdd, BDD bdd2, int maxNodes) {
      return bdd.And(bdd2, maxNodes);
    };
    merge(vars, bucket, mergeBDDs, maxTime, maxNodes);
    removeZero(bucket);  // Be sure that we do not contain only the zero BDD

    return bucket.size() <= 1;
  }

  void dumpMutexBDDs(bool fw) const;

  // Methods that require of TRs initialized
  inline int getMinTransitionCost() const {
    assert(!transitions.empty());
    return min_transition_cost;
  }

  inline int getAbsoluteMinTransitionCost() const {
    assert(!transitions.empty());
    if (hasTR0) return 0;
    return min_transition_cost;
  }

  inline bool hasTransitions0() const {
    assert(!transitions.empty());
    return hasTR0;
  }

  inline void zero_image(bool fw, const BDD &bdd, std::vector<BDD> &res,
                         int maxNodes) {
    if (fw)
      zero_image(bdd, res, maxNodes);
    else
      zero_preimage(bdd, res, maxNodes);
  }

  inline void cost_image(bool fw, const BDD &bdd,
                         std::map<int, std::vector<BDD>> &res, int maxNodes) {
    if (fw) {
      cost_image(bdd, res, maxNodes);
    } else {
      cost_preimage(bdd, res, maxNodes);
    }
  }

  BDD filter_mutex(const BDD &bdd, bool fw, int maxNodes, bool initialization);

  int filterMutexBucket(std::vector<BDD> &bucket, bool fw, bool initialization,
                        int maxTime, int maxNodes);

  inline void setTimeLimit(int maxTime) { vars->setTimeLimit(maxTime); }

  inline void unsetTimeLimit() { vars->unsetTimeLimit(); }

  friend std::ostream &operator<<(std::ostream &os,
                                  const SymStateSpaceManager &state_space);

  virtual void print(std::ostream &os, bool /*fullInfo*/) const {
    os << tag() << " (" << relevant_vars.size() << ")";
  }

  // For plan solution reconstruction. Only avaialble in original state space
  virtual const std::map<int, std::vector<TransitionRelation>>
      &getIndividualTRs() const {
    std::cerr << "Error: trying to get individual TRs from an invalid state "
                 "space type"
              << std::endl;
    utils::exit_with(utils::ExitCode::CRITICAL_ERROR);
  }

  std::shared_ptr<SymAxiomEvaluator> get_axiom_evaluator() {
    return axiomEvaluator;
  }
};

}  // namespace symbolic
#endif
