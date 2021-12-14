#ifndef CLUSTER__TRAJECTORYCLASSIFIER_H_
#define CLUSTER__TRAJECTORYCLASSIFIER_H_

#include "Data.h"

/// Псевдо-кратчайший незамкнутый путь ("жадный приближенный алгоритм поиска пути за O(n^2)")
class TrajectoryClassifier {
  std::vector<int> divisors;
  std::vector<int> trajectory;
  const Distances &ref;
 public:
  TrajectoryClassifier(const Distances &d, int k);
};

#endif //CLUSTER__TRAJECTORYCLASSIFIER_H_
