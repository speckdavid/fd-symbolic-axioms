#include "uniform_cost_search.h"

#include "closed_list.h"
#include "frontier.h"
#include "sym_solution.h"
#include "sym_util.h" 
#include "sym_engine.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

#include "../utils/debug_macros.h"
#include "../utils/timer.h"
#include "../globals.h"

#include "sym_test.h"

using namespace std;
using utils::g_timer;
using utils::Timer;

namespace symbolic {

    UniformCostSearch::UniformCostSearch(SymController * eng,
					 const SymParamsSearch & params): 
	UnidirectionalSearch(eng, params), 
	parent(nullptr), closed(make_shared<ClosedList>()),  
	estimationCost(params), estimationZero(params),
	lastStepCost(true) {}

    bool UniformCostSearch::init(std::shared_ptr<SymStateSpaceManager>  manager, 
				 bool forward, 
				 std::shared_ptr<ClosedList> closed_opposite){
	mgr = manager;
	fw = forward;
	lastStepCost = true;
	assert(mgr);
	
	DEBUG_MSG(cout << "Init exploration: " << dirname(forward) << *this/* << " with mgr: " << manager */<< endl;);

	BDD init_bdd = fw ? mgr->getInitialState() : mgr->getGoal();
	DEBUG_MSG(cout << "Init frontier: " << endl;);

	frontier.init(manager.get(), init_bdd);
	DEBUG_MSG(cout << "Init closed: " << endl;);

	closed->init(mgr.get(), this);
	closed->insert(0, init_bdd);
	closed->setHNotClosed(open_list.minNextG(frontier, mgr->getAbsoluteMinTransitionCost()));
	closed->setFNotClosed(getF());

	DEBUG_MSG(cout << "Init perfect heuristic: " << endl;);
	if (closed_opposite) {
	    perfectHeuristic = closed_opposite;
	} else if (!isAbstracted()) { 
	    if(fw){
	        perfectHeuristic = make_shared<OppositeFrontierFixed>(mgr->getGoal(), *mgr);
	    }else{
		perfectHeuristic = make_shared<OppositeFrontierFixed>(mgr->getInitialState(), *mgr);
	    }
	}

	prepareBucket();

	if(isOriginal()) engine->setLowerBound(getF());

	return true;
    }

    void UniformCostSearch::checkCutOriginal(Bucket & bucket, int g_val){
	assert(isOriginal());
	//If it is the original space, maybe we have found a solution, set upper bound  
	if(!perfectHeuristic || p.get_non_stop()){
	    return;
	}

	for (BDD & bucketBDD : bucket){
	    SymSolution sol = perfectHeuristic->checkCut(this, bucketBDD, g_val, fw);
	    if (sol.solved()){
		cout << "Solution found with cost " << sol.getCost() << 
		    " total time: " << g_timer <<  endl;
		// Solution found :)
		engine->new_solution(sol);
	    }
	    bucketBDD *= perfectHeuristic->notClosed();   //Prune everything closed in opposite direction
	}
    }

    void UniformCostSearch::prepareBucket(){
	if(!frontier.bucketReady()){
	    DEBUG_MSG(cout << "POP: bucketReady: " << frontier.bucketReady() << endl;);

	    if(open_list.empty()){
		closed->setHNotClosed(numeric_limits<int>::max());
		closed->setFNotClosed(numeric_limits<int>::max());
		if(isOriginal()) engine->setLowerBound(getF());
		return; //Search finished
	    }

	    open_list.pop(frontier);
	    assert(!frontier.empty() || frontier.g() == numeric_limits<int>::max() );
	    if (mgr->isOriginal()) {
		checkCutOriginal(frontier.bucket(), frontier.g());
	    }
	    frontier.filter(!closed->notClosed());
	    if (perfectHeuristic && perfectHeuristic->exhausted()) {
		frontier.filter(perfectHeuristic->notClosed());
	    }
	    mgr->filterMutex(frontier.bucket(), fw, initialization());
	    removeZero(frontier.bucket());
	
	    // Close and move to reopen

	    if(isAbstracted() || !lastStepCost || frontier.g() != 0){
		//Avoid closing init twice
		DEBUG_MSG (cout <<"Insert g="<< frontier.g() << " states into closed: " << 
			   nodeCount(frontier.bucket()) << " (" << frontier.bucket().size() << " bdds)" << endl;);
		for(const BDD & states : frontier.bucket()){
		    DEBUG_MSG (cout <<"Closing: " << states.nodeCount() << endl;);

		    closed->insert(frontier.g(), states);
		}
	    } 

	    closed->setHNotClosed(open_list.minNextG(frontier, mgr->getAbsoluteMinTransitionCost()));
	    closed->setFNotClosed(getF());
	    if(isOriginal()) engine->setLowerBound(getF());
   
	    computeEstimation(true);
	}

	if(engine->solved()){
	    DEBUG_MSG(cout << "SOLVED!!!: "<< engine->getLowerBound() << " >= " << engine->getUpperBound() << endl;);
	    return; //If it has been solved, return 
	}

	initialization();

	int maxTime = p.getAllotedTime(nextStepTime());
	int maxNodes = p.getAllotedNodes(nextStepNodesResult());

        Result res = frontier.prepare(maxTime, maxNodes, fw, initialization()); 
	if(!res.ok) {
	    violated(res.truncated_reason, res.time_spent, maxTime, maxNodes);
	}
    }

    bool UniformCostSearch::stepImage(int maxTime, int maxNodes){
	if(p.debug) {
	    cout << ">> Step: " << *mgr << (fw ? " fw " : " bw ") << ", g=" << frontier.g()
		 << " frontierNodes: " << frontier.nodes() << " [" << frontier.buckets() << "]" 
		 << " total time: " << g_timer 
		 << " total nodes: " << mgr->totalNodes() 
		 << " total memory: " << mgr->totalMemory() << endl;
    }

#ifdef DEBUG_GST
	gst_plan.checkUcs(this );
#endif

	DEBUG_MSG(cout << "Step " << dirname(fw)  << " g: " << frontier.g() << endl;);
	Timer sTime;
	DEBUG_MSG(cout << "preparing bucket.." << " total time: " << g_timer  << endl;);
	Result prepare_res = frontier.prepare(maxTime, maxNodes, fw, initialization()); 
	if(!prepare_res.ok){
	    violated(prepare_res.truncated_reason, prepare_res.time_spent, maxTime, maxNodes);
	    cout << "    >> Truncated while preparing bucket" << endl;
	    if(sTime()*1000.0 > p.maxStepTime){
		double ratio = (double)p.maxStepTime/((double)sTime()*1000.0);
		p.maxStepNodes *= ratio;
		DEBUG_MSG(cout << "MAX STEP NODES CHANGED TO: " << p.maxStepNodes <<
			  " after truncating with " << sTime() << " seconds" << endl;);
	    }
	    stats.step_time += sTime();
	    return false;    
	} 
	DEBUG_MSG(cout << "... bucket prepared. " << endl;);
	if(engine->solved()) return true; //Skip image if we are done

	int stepNodes = frontier.nodes();
	ResultExpansion res_expansion = frontier.expand(maxTime, maxNodes, fw);

	if(res_expansion.ok) {
	    lastStepCost = false; //Must be set to false before calling checkCut
	    //Process Simg, removing duplicates and computing h. Store in Sfilter and reopen.
	    //Include new states in the open list 
	    for(auto & resImage : res_expansion.buckets){
		for(auto & pairCostBDDs : resImage){
		    int cost = frontier.g() + pairCostBDDs.first; //Include states of the given cost 
		    mgr->mergeBucket(pairCostBDDs.second);

		    //Check the cut (removing states classified, since they do not need to be included in open)
		    if (!isAbstracted()){
			checkCutOriginal(pairCostBDDs.second, cost); 
		    }

		    for(auto & bdd : pairCostBDDs.second){  
			if(!bdd.IsZero()){
			    //TODO: maybe we can also use the heuristics to prune states
			    //right here. Also, we could prune duplicates. Not sure if it
			    //is worth it.
			    int fVal = cost;
			    if(perfectHeuristic){
				fVal += perfectHeuristic->getHNotClosed();
			    }
			    if (fVal < engine->getUpperBound()) {
				stepNodes = max(stepNodes, bdd.nodeCount());
				open_list.insert(bdd, cost);
			    }
			}
		    }
		}
	    }
	    stats.add_image_time(res_expansion.time_spent);
	} else {
	    stats.add_image_time_failed(res_expansion.time_spent);
	} 

	if (!res_expansion.step_zero) {
	    estimationCost.stepTaken(1000*res_expansion.time_spent, stepNodes);
	}else { 
	    estimationZero.stepTaken(1000*res_expansion.time_spent, stepNodes);
	}

	//Try to prepare next Bucket
	computeEstimation(true);

	//We prepare the next bucket before checking time in doing
	//the step because we consider preparing the bucket as a
	//part of the step.
	prepareBucket();

	if(sTime()*1000.0 > p.maxStepTime){
	    double ratio = (double)p.maxStepTime/((double)sTime()*1000.0);
	    p.maxStepNodes = stepNodes*ratio;
	    DEBUG_MSG(cout << "MAX STEP NODES CHANGED TO: " << p.maxStepNodes << " after taking " << sTime() << " seconds" << endl;);
	}else if (!res_expansion.ok){
	    //In case maxAllotedNodes were exceeded we reduce the maximum
	    //frontier size by 3/4.  TODO: make this a parameter
	    p.maxStepNodes = stepNodes*0.75; 
	}

	stats.step_time += sTime();
	return res_expansion.ok;
    }

    bool UniformCostSearch::isSearchableWithNodes(int maxNodes) const{   
	return frontier.expansionReady() && nextStepNodes() <= maxNodes;
    }


    void UniformCostSearch::computeEstimation(bool prepare){
	if(prepare){
	    prepareBucket(/*p.max_pop_time, p.max_pop_nodes, true*/);
	    DEBUG_MSG(cout << " bucket prepared for compute estimation" << endl;);
	}

	if(frontier.expansionReady()){
	    //Succeded, the estimation will be only in image
	    if(frontier.nextStepZero()){
		estimationZero.nextStep(frontier.nodes());
	    }else{
		estimationCost.nextStep(frontier.nodes());
	    }
	}else{
	    if (mgr->hasTransitions0()){
		estimationZero.nextStep(frontier.nodes());
	    }else{
		estimationCost.nextStep(frontier.nodes());
	    }
	} 
	DEBUG_MSG(cout << "estimation computed" << endl;);
    }


    long UniformCostSearch::nextStepTime() const{
	long estimation = 0;

	if(mgr->hasTransitions0() && (!frontier.expansionReady() || frontier.nextStepZero())){
	    estimation += estimationZero.time();
	}else {
	    estimation += estimationCost.time();
	}
	return estimation;
    }

    long UniformCostSearch::nextStepNodes() const {
	if(mgr->hasTransitions0() &&  (!frontier.expansionReady() || frontier.nextStepZero())){
	    return estimationZero.nextNodes();
	}else {
	    return estimationCost.nextNodes();
	}  
    }

    long UniformCostSearch::nextStepNodesResult() const {
	long estimation = 0;
  
	if(mgr->hasTransitions0() && (!frontier.expansionReady() || frontier.nextStepZero())){
	    estimation = max(estimation, estimationZero.nodes());
	}else {
	    estimation = max(estimation, estimationCost.nodes());
	}
	return estimation;
    }

    ADD UniformCostSearch::getHeuristic() const{
	return closed->getHeuristic();
    }


/////////////////////////////////////////////////
////   Auxiliar methods to load/save/print   ////
/////////////////////////////////////////////////

    std::ostream & operator<<(std::ostream &os, const UniformCostSearch & exp){
	os << "exp " << dirname(exp.isFW());
	if(exp.mgr){
	    os << " in " << *(exp.mgr)
	       << " f=" << exp.getF() << flush
	       << " g=" << exp.frontier.g() << flush
	       << exp.open_list << flush
	       << " est_time: " << exp.nextStepTime() << flush
	       << " est_nodes: " << exp.nextStepNodes() <<flush;  
	}
	return os;
    }

    
    void UniformCostSearch::violated(TruncatedReason reason, double ellapsed_seconds, int maxTime, int maxNodes){
	//DEBUG_MSG(
	cout << "Truncated in " << reason << ", took " << ellapsed_seconds << " s," << 
	    " maxtime: " << maxTime << " maxNodes: " << maxNodes<< endl;
	//);
	int time = 1 + ellapsed_seconds*1000;

	if(mgr->hasTransitions0() && 
	   (!frontier.expansionReady() || frontier.nextStepZero())){
	    estimationZero.violated(time, maxTime, maxNodes);
	}else{
	    estimationCost.violated(time, maxTime, maxNodes);
	}
    }

    BDD UniformCostSearch::getClosedTotal() {
	return closed->getClosed();
    }

    BDD UniformCostSearch::notClosed(){
	return closed->notClosed();
    }

    std::ostream & operator<<(std::ostream &os, const TruncatedReason & reason){
	switch(reason){
	case TruncatedReason::FILTER_MUTEX:
	    return os << "filter_mutex";
	case TruncatedReason::MERGE_BUCKET:
	    return os << "merge";
	case TruncatedReason::MERGE_BUCKET_COST:
	    return os << "merge_cost";
	case TruncatedReason::IMAGE_ZERO: 
	    return os << "0-image";
	case TruncatedReason::IMAGE_COST:
	    return os << "cost-image";
	default: 
	    cerr << "UniformCostSearch truncated by unkown reason" << endl;
	    utils::exit_with(utils::ExitCode::UNSUPPORTED);
	}
    }

    void UniformCostSearch::getPlan(const BDD &cut, int g, std::vector <const GlobalOperator *> &path) const {

	closed->extract_path(cut, g, fw, path);
    	if (fw) {  
    	    std::reverse(path.begin(), path.end());
    	} 
    }
}
