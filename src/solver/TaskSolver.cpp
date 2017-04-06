#include "TaskSolver.hpp"

using namespace std;

void TaskSolver::preSolve(){
	_layouts.reserve(_taskNb);
	_qualities.reserve(_taskNb);
	for(int i = 0 ; i < _taskNb ; ++i)
		_layouts.emplace_back(_shapes);
}

void TaskSolver::solveBin(){
	#pragma omp parallel for schedule(static)
	for(int i = 0 ; i < _taskNb ; ++i){
		#pragma omp task
		{
			ProbaSolver s(_layouts[i], {});
			s.solve();
			_qualities[i] = _layouts[i].quality();
		}
	}
	#pragma omp barrier
	//Do the flop
	unsigned i = distance(_qualities.begin(), max_element(_qualities.begin(), _qualities.end()));
	Solution finale;
	_layouts[i].genSolution(finale);
	_shapes.applySolution(finale);
}
