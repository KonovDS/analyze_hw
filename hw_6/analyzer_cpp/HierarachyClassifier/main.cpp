#include <iostream>

#include "Data.h"
#include "HierarchyClassifier.h"

int main() {
  Distances kos("dist.kos.txt");
  Distances nips("dist.nips.txt");
  HierarchyClassifier nips_c(nips, 300, 10);
  HierarchyClassifier kos_c(kos, 300, 10);
  return 0;
}
