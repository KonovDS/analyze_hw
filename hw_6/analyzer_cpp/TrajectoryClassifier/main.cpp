#include <iostream>

#include "Data.h"
#include "TrajectoryClassifier.h"

int main() {
  Distances kos("dist.kos.txt");
  TrajectoryClassifier k_tc(kos, 10);
  Distances nips("dist.nips.txt");
  TrajectoryClassifier n_tc(nips, 10);
  return 0;
}
