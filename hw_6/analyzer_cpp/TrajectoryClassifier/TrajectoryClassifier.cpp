#include "TrajectoryClassifier.h"

#include <iostream>
#include <algorithm>

int FindMin(const Distances &d, int i, const std::vector<bool> &mask) {
  int res = INT16_MAX;
  int res_index = -1;
  for (int j = 0; j < d.articles; j++) {
    if (mask[j] || i == j)
      continue;
    if (res > d.At(i, j)) {
      res = d.At(i, j);
      res_index = j;
    }
  }
  return res_index;
}

TrajectoryClassifier::TrajectoryClassifier(const Distances &d, int k): ref(d) {
  std::cout << "[NOTICE] Starting trajectory classifier learning." << std::endl;
  std::vector<bool> visited = std::vector<bool>(ref.Articles(), false);

  // Поиск минимального пути
  std::pair<int, int> min_index = {1, 0};
  for (int i = 0; i < ref.Articles(); i++) {
    for (int j = i + 1; j < ref.Articles(); j++) {
      if (ref.At(i, j) < ref.At(min_index.first, min_index.second)) {
        min_index = {i, j};
      }
    }
  }

  std::cout << "[NOTICE] Progress:                            end \\/" << std::endl;
  trajectory = std::vector<int>();
  { // Построение пути
    trajectory.push_back(min_index.first);
    trajectory.push_back(min_index.second);
    visited[min_index.first] = true;
    visited[min_index.second] = true;
    int j = min_index.second;
    int i = 0;
    while (j = FindMin(ref, j, visited), j != -1) {
      if (i++ % (ref.Articles() / 50) == 0) {
        std::cout << "-";
      }
      visited[j] = true;
      trajectory.push_back(j);
    }
  }
  divisors = std::vector<int>(k, 0);
  { // Поиск максимумов в пути и разделение пути
    for (int i = 0; i < d.Articles() - 1; i++) {
      for (int j = 0; j < k; j++) {
        int l = divisors[j];
        if (ref.At(trajectory[l], trajectory[l+1]) < ref.At(trajectory[i], trajectory[i+1])) {
          divisors[j] = i;
          break;
        }
      }
    }
  }

  // Сортировка массива разделителей по длине
  /*
  std::sort(divisors.begin(), divisors.end(), [this](auto a, auto b) {
    return this->ref.At(ref.At(trajectory[a], trajectory[a+1])) < this->ref.At(ref.At(trajectory[b], trajectory[b+1]));
  });
  */

  // Сортировка массива разделителей по положению
  std::sort(divisors.begin(), divisors.end(), [](auto a, auto b) {
    return a < b;
  });

  std::cout << "\n[NOTICE] Done dividing in " << k << " classes." << std::endl;
  std::cout << "[NOTICE] {class, number of elements}: \n{" << 0 << ", " << divisors[0] + 1 << "}, ";
  for (int i = 1; i < k - 1; i++) {
    std::cout << "{" << i << ", " << divisors[i] - divisors[i - 1] << "}, ";
  }
  std::cout << "{" << k << ", " << ref.Articles() - divisors[k-1] << "}" << std::endl;
}

