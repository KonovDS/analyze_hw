#ifndef CLUSTER__DATA_H_
#define CLUSTER__DATA_H_

#include <vector>
#include <string>

class Distances {
 public:
  int articles;
  std::vector<std::vector<int>> d;
  Distances(const std::string &path);
  int At(int i, int j) const;
  int Articles() const { return articles; }
  void BarChartOut(const std::string &out, int max) const;
  friend int FindMin(const Distances &d, int i, const std::vector<bool> &mask);
};

#endif //CLUSTER__DATA_H_
