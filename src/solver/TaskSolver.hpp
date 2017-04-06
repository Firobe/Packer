#ifndef TASKSOLVER_HPP
#define TASKSOLVER_HPP

#include "Solver.hpp"
#include "ProbaSolver.hpp"

#define DEFAULT_TASK_NB (4)

class TaskSolver : public Solver {
    void solveBin() override;
    void preSolve() override;
	int _taskNb;
	std::vector<Layout> _layouts;
	std::vector<double> _qualities;
	std::vector<Parameter> _params;
public:
    TaskSolver(Layout& v, const std::vector<Parameter>& p) : Solver(v, p), _params(p) {
        if (!getParameter(p, "task_nb", _taskNb))
			_taskNb = DEFAULT_TASK_NB;
    }
};

#endif
