#include "HierarchyClassifier.h"

#include <iostream>

#include <omp.h>

/// Расстояние между классом и нодой (Среднее)
/// \param i - номер класса
/// \param j - номер ноды
/// \return - расстояние
double HierarchyClassifier::Distance(int i, int j) const {
  int ret = 0;
  int n = 0;
  for (int k = 0; k < art; k++) {
    if (std::abs(c[k]) == i) {
      ret += ref.At(k, j);
      n++;
    }
  }
  return static_cast<double>(ret)/n;
}

/// Поиск пары класс-обьект с минимальной дистаницей O(n^3)
/// \param m - маска классов
/// \return пара класс - обьект
std::pair<int, int> HierarchyClassifier::MinDistance(const std::vector<bool> &m) const {
  double d = 1e9;
  std::pair<int, int> ret;
  for (int i = 0; i < art; i++) {
    if (c[i] > 0) { // Для всех классов, в том числе состоящих из одного обьекта
      for (int j = 0; j < art; j++) {
        if (c[j] > 0 && !m[j] && i != j) { // Только для классов с одним обьектом
          if(Distance(c[i], j) < d) {
            d = Distance(c[i], j);
            ret = {c[i], j};
          }
        }
      }
    }
  }
  return ret;
}

HierarchyClassifier::HierarchyClassifier(Distances &d, int articles, int k) : ref(d) {
  std::cout << "[NOTICE] Starting HierarchyClassifier" << std::endl;
  art = articles;
  c = std::vector<int>(art);
  auto m = std::vector<bool>(art, false);
  // В начале все обьекты в своем отдельном классе
  for (int i = 0; i < art; i++) {
    c[i] = i + 1;
  }
  std::cout << "[NOTICE] Progress:                            end \\/" << std::endl;
  // Теперь ищем самые ближайшие классы.
  // Один из классов должен содержать всего один обьект!
  // Обьединяем эти классы.
  // Для каждого элемента кроме одного выставляем отрицательную метку класса
  // Для первого элемента класса устанавливаем true в маске

  // O(n^4)
  for (int i = 1; i < art - k; i++) {
    if (i % ((art - 1 - k)/50) == 0) {
      std::cout << "-";
    }
    auto r = MinDistance(m);
    m[r.first] = true;
    c[r.second] = -(c[r.first]);
  }
  std::cout << "\n[NOTICE] Done!" << std::endl;
  std::cout << "[NOTICE] Class, number of entries:" << std::endl;
  auto nums = std::vector<int>(art, 0);
  for (int i = 0; i < art; i++) {
    nums[std::abs(c[i])]++;
  }
  for (int i = 0; i < art; i++) {
    if(nums[i] != 0 && nums[i] != 1) {
      std::cout << i << ", " << nums[i] << std::endl;
    }
  }
}