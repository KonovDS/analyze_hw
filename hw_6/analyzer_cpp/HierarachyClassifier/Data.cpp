#include "Data.h"

#include <iostream>
#include <fstream>

int Distances::At(int i, int j) const {
  int ii = std::min(i, j);
  int jj = std::max(i, j);
  return d[ii][jj - ii];
}

Distances::Distances(const std::string &path) {
  std::cout << "[NOTICE] Reading file \"" << path << "\"." << std::endl;
  std::ifstream f(path);
  f >> articles;
  d = std::vector<std::vector<int>>(articles);
  std::cout << "[NOTICE] Progress:                            end \\/" << std::endl;
  for (int i = 0; i < articles; i++) {
    d[i] = std::vector<int>(articles - i);
    d[i][0] = 0;
  }
  for (int i = 0; i < articles; i++) {
    if (i % (articles/50) == 0) {
      std::cout << "-";
    }
    for (int j = i + 1; j < articles; j++) {
      f >> d[i][j-i];
    }
  }
  f.close();
  std::cout << "\n[NOTICE] Success" << std::endl;
}
