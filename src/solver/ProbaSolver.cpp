#include <algorithm>
#include <random>

#include "ProbaSolver.hpp"
#include "Shape.hpp"
#include "MultilineSolver.hpp"

using namespace std;

void ProbaSolver::preSolve() {
    random_shuffle(_shapes.begin(), _shapes.end());
	MultilineSolver s(_shapes, {{"sort", 0}});
	s.solve();
	_binNumber = s.getBinNb();
	_centroids.reserve(_shapes.size());
	for(Shape& s : _shapes){
		_centroids.emplace_back(s.centroid());
	}
}

void ProbaSolver::solveBin() {
	for(int i = 0 ; i < 100 ; ++i)
		nextStep();
	_indices.clear();
}

double ProbaSolver::swagRNG() const {
	static default_random_engine generator;
	static exponential_distribution<double> distribution(_amplitudeProba);
	return distribution(generator);
}

/**
 * Next iteration
 */
void ProbaSolver::nextStep() {
	for(unsigned i = 0 ; i < _shapes.size() ; ++i){
		double angle = swagRNG() * 10;
		double tX = (1 - (rand() % 2) * 2) * swagRNG() * Parser::getDims().x(),
				 tY = (1 - (rand() % 2) * 2) * swagRNG() * Parser::getDims().y();
		_shapes[i].translate(-_centroids[i].x(), -_centroids[i].y());
		_shapes[i].rotate(angle);
		_shapes[i].translate(_centroids[i].x() + tX, _centroids[i].y() + tY);

		translate(_centroids[i], tX, tY);
		for(unsigned j = 0 ; j < i ; ++j)
			if(_shapes[i].intersectsWith(_shapes[j])){
				_shapes[i].translate(-_centroids[i].x(), -_centroids[i].y());
				_shapes[i].rotate(-angle);
				_shapes[i].translate(_centroids[i].x(), _centroids[i].y());
				break;
			}
	}
}
