#ifndef SYMBOLIC_SYM_ENUMS_H
#define SYMBOLIC_SYM_ENUMS_H

#include <iostream>
#include <string>
#include <vector>

// Auxiliar file to declare all enumerate values Each enumerate has:
// its definiton, ostream << operator and an array with representattive
// strings. The order of the values in the enumerate must correspond
// with the order in the xxxValues vector

namespace symbolic {
enum class AxiomType {
  INTERLEAVED,  // 
  INTERLEAVED_RULES,  // Option 1
  PRECOMPUTED,  // Option 1 but precomputed on the constant 1 BDD
  VARBDDS,      // Option 2
  TRANSFORM     // Option 3
};
std::ostream &operator<<(std::ostream &os, const AxiomType &a);
extern const std::vector<std::string> AxiomTypeValues;

/* enum class AxiomStratification { */
/*   DEFAULT,             // FD stratification (if one) */
/*   TRANSITIVE_CLOSURE,  // Based on transitive closure ("In defense of axioms") */
/* }; */
/* std::ostream &operator<<(std::ostream &os, const AxiomStratification &a); */
/* extern const std::vector<std::string> AxiomStratificationValues; */

enum class MutexType {
  MUTEX_NOT,
  MUTEX_AND,
  MUTEX_RESTRICT,
  MUTEX_NPAND,
  MUTEX_CONSTRAIN,
  MUTEX_LICOMP,
  MUTEX_EDELETION
};
std::ostream &operator<<(std::ostream &os, const MutexType &m);
extern const std::vector<std::string> MutexTypeValues;

enum class AbsTRsStrategy {
  TR_SHRINK,
  IND_TR_SHRINK,
  REBUILD_TRS,
  SHRINK_AFTER_IMG
};
std::ostream &operator<<(std::ostream &os, const AbsTRsStrategy &a);
extern const std::vector<std::string> AbsTRsStrategyValues;

enum class LinearPDBStrategy { LEVEL, REVERSE, BINARY };
std::ostream &operator<<(std::ostream &os, const LinearPDBStrategy &st);
extern const std::vector<std::string> LinearPDBStrategyValues;

// Types of strategies to choose in which direction search the abstract state
// spaces
enum class RelaxDirStrategy { FW, BW, BIDIR, SWITCHBACK };
std::ostream &operator<<(std::ostream &os, const RelaxDirStrategy &relaxDir);
extern const std::vector<std::string> RelaxDirStrategyValues;

enum class Dir { FW, BW, BIDIR };
std::ostream &operator<<(std::ostream &os, const Dir &dir);
extern const std::vector<std::string> DirValues;

enum AbsMinimizationType {
  APPROX,
  REMAP_UNDER_APPROX,
  HEAVY_BRANCH,
  SHORT_PATHS
};
std::ostream &operator<<(std::ostream &os, const AbsMinimizationType &dir);
extern const std::vector<std::string> AbsMinimizationTypeValues;

enum UCTRewardType {
  STATES,
  NODES,
  STATES_TIME,
  NODES_TIME,
  STATES_NODES,
  NONE,
  RAND
};
std::ostream &operator<<(std::ostream &os, const UCTRewardType &dir);
extern const std::vector<std::string> UCTRewardTypeValues;
}  // namespace symbolic
#endif
