#ifndef SEARCH_ENGINES_SYMBOLIC_SEARCH_H
#define SEARCH_ENGINES_SYMBOLIC_SEARCH_H

#include <vector>
#include <memory>

#include "../symbolic/sym_controller.h"
#include "../symbolic/sym_enums.h"
#include "../search_engine.h"

namespace options {
    class Options;
}

namespace symbolic {
    class SymStateSpaceManager;
    class SymSearch;
    class SymSolution;

}

namespace symbolic_search {

	enum class SearchDirection {
		NONE,
		FWD,
		BWD,
		BID
	};

	extern SearchDirection g_symbolic_search_direction;


    class SymbolicSearch : public SearchEngine, public symbolic::SymController { 
    protected:
	// Symbolic manager to perform bdd operations
	std::shared_ptr<symbolic::SymStateSpaceManager> mgr; 

	std::unique_ptr<symbolic::SymSearch> search;

	virtual SearchStatus step() override;

    public:
	SymbolicSearch(const options::Options &opts);
	virtual ~SymbolicSearch() = default;

	virtual void new_solution(const symbolic::SymSolution & sol) override;
    };


    class SymbolicBidirectionalUniformCostSearch : public SymbolicSearch { 
    protected:
	virtual void initialize() override;
	
    public:
	SymbolicBidirectionalUniformCostSearch(const options::Options &opts);
	virtual ~SymbolicBidirectionalUniformCostSearch() = default;
    };

    
    class SymbolicUniformCostSearch : public SymbolicSearch { 
	bool fw;
    protected:
	virtual void initialize() override;
	
    public:
	SymbolicUniformCostSearch(const options::Options &opts, bool _fw);
	virtual ~SymbolicUniformCostSearch() = default;
    };

    
    
}

#endif
