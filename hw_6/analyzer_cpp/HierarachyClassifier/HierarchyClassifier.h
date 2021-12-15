#ifndef CLUSTER__HIERARCHYCLASSIFIER_H_
#define CLUSTER__HIERARCHYCLASSIFIER_H_

#include <vector>

#include "Data.h"

class HierarchyClassifier {
  std::vector<int> c;
  Distances &ref;
  int art;
 public:
  HierarchyClassifier(Distances &d, int articles, int k);
  double Distance(int i, int j) const;
  std::pair<int, int> MinDistance(const std::vector<bool> &m) const;
};

#endif //CLUSTER__HIERARCHYCLASSIFIER_H_
