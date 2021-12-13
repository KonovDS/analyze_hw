#ifndef CLUSTER__DATA_H_
#define CLUSTER__DATA_H_

#include <vector>
#include <string>

class RawData {
  int articles;
  std::vector<std::vector<int>> d;
  int Distance(int i, int j) const;
 public:
  RawData(const std::string &txt);
  friend class Distances;
};

class Distances {
  int articles;
  std::vector<std::vector<int>> d;
 public:
  Distances(const RawData &r);
  Distances(const std::string &path);
  void BarChartOut(const std::string &out, int max) const;
  int At(int i, int j) const;
  int Articles() const { return articles; }
  void ToFile(const std::string &s) const;
};

bool Equal(const Distances &a, const Distances &b);

#endif //CLUSTER__DATA_H_
