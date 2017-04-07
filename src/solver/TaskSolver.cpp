#include "TaskSolver.hpp"
#include "Log.hpp"

using namespace std;

void TaskSolver::preSolve(){
	_layouts.reserve(_taskNb);
	_qualities.resize(_taskNb);
	for(int i = 0 ; i < _taskNb ; ++i){
		_layouts.emplace_back(_shapes);
	}
}

void TaskSolver::solveBin(){
	#pragma omp parallel for schedule(static)
	for(int i = 0 ; i < _taskNb ; ++i){
		#pragma omp task firstprivate(i)
		{
			ProbaSolver s(_layouts[i], _params);
			s.solve();
			_qualities[i] = _layouts[i].quality();
		}
	}
	#pragma omp barrier
	//Do the flop
	unsigned i = distance(_qualities.begin(), max_element(_qualities.begin(), _qualities.end()));
	Solution finale;
    sort(_shapes.begin(), _shapes.end(), [](const Shape & a, const Shape & b) {
        return a.getID() < b.getID();
    });
    sort(_layouts[i].begin(), _layouts[i].end(), [](const Shape & a, const Shape & b) {
        return a.getID() < b.getID();
    });
	_layouts[i].genSolution(finale);
	_shapes.applySolution(finale);
	_indices.clear();
}
