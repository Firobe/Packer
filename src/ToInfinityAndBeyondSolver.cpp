#include "ToInfinityAndBeyondSolver.h"
#include <vector>
#include "types.h"

using namespace std;

void ToInfinityAndBeyondSolver::solve(){
  //_shapes[pouet] //contient l'ensemble des objets
    vector<Box> boxes(_shapes.size());
  Box boxAux = Box();
  Ring shapeAux = _shapes[0].getRing();
  for(unsigned i = 0; i < _shapes.size(); i++){
    bg::envelope(_shapes[i].getRing(), boxes[i]);
  }

  Point reference = boxes[0].min_corner();
  boost::geometry::strategy::transform::translate_transformer<double, 2, 2>translate(-reference.x(),-reference.y());
  boost::geometry::transform(_shapes[0].getRing(), shapeAux, translate);
  boost::geometry::transform(boxes[0], boxAux, translate);
  _shapes[0].getRing() = shapeAux;
  boxes[0] = boxAux;

  Point previous;
  for(unsigned i = 1; i < _shapes.size(); i++){
    reference = boxes[i].min_corner();
    previous = boxes[i-1].max_corner();
    Ring shapeAux = _shapes[i].getRing();
    boost::geometry::strategy::transform::translate_transformer<double, 2, 2>translate(previous.x()-reference.x(),-reference.y());
    boost::geometry::transform(_shapes[i].getRing(), shapeAux, translate);
    boost::geometry::transform(boxes[i], boxAux, translate);
    _shapes[0].getRing() = shapeAux;
    boxes[0] = boxAux;
  }

}

