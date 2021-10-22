#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

#define NUM_FEATURES 104
#define TRAIN_SIZE 30162
#define TEST_SIZE 15060

#define TREE_DEPTH 2

int LineGet(const std::string &s, std::vector<int> &a, int num_features) {
  int j = 0, pos = 0;
  for(int i = 0; i < num_features; i++)
  {
    while (s[j] != ',')
      j++;
    a[i] = std::stoi(s.substr(pos, j - pos));
    pos = ++j;
  }
  while (s[j] != ',')
    j++;
  return std::stoi(s.substr(pos, j - pos));
}

class Data {
 public:
  // std::array<std::string, NUM_FEATURES + 1> train_names;
  // std::array<std::string, NUM_FEATURES + 1> test_names;
  std::vector<std::vector<int>> x;
  std::vector<int> y;
  // friend class DataSelector;
 public:
  Data(const std::string &csv, int size, int num_features): x(size), y(size) {
    std::ifstream fs;
    fs.open(csv);
    if (!fs.is_open()) {
      std::cout << "[ERROR] Unable to open train file \"" << csv << "\"" << std::endl;
      exit(-1);
    }
    int i = 0;
    int num_read = 0;
    std::string line;
    // Первая строка с названиями столбцов
    std::getline(fs, line);
    while (std::getline(fs, line)) {
      num_read++;
      x[i] = std::vector<int>(num_features);
      y[i] = LineGet(line, x[i++], num_features);
    }
    fs.close();
    std::cout << "[NOTICE] Read \"" << csv << "\". Total " << num_read << " entries." << std::endl;
  }
};

int Pow2(int num) {
  return std::pow(2, num);
}

// Приводим дерево к вектору

template<typename T>
class MyTree {
 public:
  std::vector<T> v;
  int depth;
  explicit MyTree(int d) : v(Pow2(d) - 1), depth(d) {
    std::cout << "[NOTICE] Allocated tree with " << d << " depth and " << Pow2(d) - 1 << " leaves." << std::endl;
  }
};

template<typename T>
class MyIterator {
  int r_d, i;
  MyTree<T> &tree;

  int GetTreeIndex(int remaining_depth, int index, bool is_left) const {
    if(remaining_depth == 1) {
      std::cout << "[ERROR] No more children of leaf" << std::endl;
      exit(-1);
    }
    /*
    std::cout << "rd = " << remaining_depth << " index = " << index
              << " is_left = " << is_left << " answer = "
              << ((is_left)?(index+1):(index + Pow2(remaining_depth - 1))) << std::endl;
              */
    if(is_left)
      return index + 1;
    return index + Pow2(remaining_depth - 1);
  }

 public:
  explicit MyIterator(MyTree<T> &t, int depth = -1, int index = 0): tree(t) {
    if (depth == -1) {
      r_d = t.depth;
    } else {
      r_d = depth;
    }
    i = index;
  }
  T &el() {
    return tree.v[i];
  }
  MyIterator<T> Next(bool is_left) const {
    return MyIterator(tree, r_d - 1, GetTreeIndex(r_d, i, is_left));
  }
  void Move(bool is_left) {
    i = GetTreeIndex(r_d--, i, is_left);
  }
  int GetRemainingDepth() const {
    return r_d;
  }
};

struct MyNode {
  int feature_num;
  // Если feature_num = -1, то крайний лист дерева
  // Тогда classification - номер класса
  union {
    int treshold;
    int classification;
  } u;
  MyNode() {
    feature_num = 1;
    u.treshold = 0;
  };
};

class DataSelector {
 public:
  std::vector<bool> selected;
  const Data &data;
  DataSelector(int entries, const Data &d): data(d), selected(entries) {
    for(int i = 0; i < entries; i++)
      selected[i] = true;
  }

  void Unselect(const MyNode &n, bool is_left) {
    for(int i = 0; i < selected.size(); i++) {
      if (!selected[i])
        continue;
      if ((data.x[i][n.feature_num] < n.u.treshold) != is_left)
        selected[i] = false;
    }
  }

  DataSelector(const DataSelector& s): data(s.data) {
    selected = s.selected;
  }

  int Length() const {
    int ret = 0;
    for (int i = 0; i < selected.size(); i++) {
      if (selected[i])
        ret++;
    }
    return ret;
  }

};

class DataSelectorIterator {
 public:
  DataSelector &d_s;
  int i, len;
  DataSelectorIterator(DataSelector &ref): d_s(ref) {
    i = 0;
    len = d_s.selected.size();
    while(i < len && d_s.selected[i] == 0) {
      i++;
    }
  }
  void Increment() {
    i++;
    while(i < len && d_s.selected[i] == 0) {
      i++;
    }
  }
  bool IsValid() const {
    return i < len;
  }
  const std::vector<int> &at() {
    return d_s.data.x[i];
  }
  int at_class() {
    return d_s.data.y[i];
  }
};

int Classify(const std::vector<int> &x, MyTree<MyNode> &tree) {
  MyIterator<MyNode> leaf(tree);
  int i = 0;
  while(leaf.GetRemainingDepth() != 1)
  {
    leaf.Move(x[leaf.el().feature_num] < leaf.el().u.treshold);
  }
  return leaf.el().u.classification;
}

class RandomUniform {
  std::random_device rd;
  std::mt19937 gen;
 public:
  RandomUniform(): rd() {
    gen = std::mt19937(rd());
  }
  int GetRandFromVectorAndErase(std::vector<int> &include) {
    auto d = std::uniform_int_distribution<>(0, include.size() - 1);
    int index = d(gen);
    int ret = include[index];
    include.erase(include.begin()+index);
    return ret;
  }
};

struct Segment {
  int a, b;
};

Segment GetSegment(DataSelector &d_s, int num_feature) {
  DataSelectorIterator d_i(d_s);
  Segment ret = {d_i.at()[num_feature], d_i.at()[num_feature]};
  while(d_i.IsValid())
  {
    if (d_i.at()[num_feature] < ret.a)
    {
      ret.a = d_i.at()[num_feature];
    } else if (d_i.at()[num_feature] > ret.b) {
      ret.b = d_i.at()[num_feature];
    }
    d_i.Increment();
  }
  return ret;
}

int Count(DataSelector &d_s, int num_feature, int treshold) {
  DataSelectorIterator d_i(d_s);
  int false_negative = 0;
  while(d_i.IsValid())
  {
    if (d_i.at()[num_feature] < treshold)
      false_negative += d_i.at_class();
    d_i.Increment();
  }
  return false_negative;
}

int FindOptimalTreshold(DataSelector &d_s, int num_feature) {
  int num = d_s.Length();
  Segment seg = GetSegment(d_s, num_feature);
  int best_treshold = seg.a;
  int c = Count(d_s, num_feature, best_treshold);
  int best_count = std::min(c, num - c);
  for(int i = seg.a + 1; i <= seg.b + 1; i++)
  {
    int c = Count(d_s, num_feature, i);
    if(std::min(c, num - c) <= best_count) {
      best_count = std::min(c, num - c);
      best_treshold = i;
    }
  }
  return best_treshold;
}

int CountClass(DataSelector &d_s, MyTree<MyNode> &tree) {
  int ret = 0;
  DataSelectorIterator i(d_s);
  while(i.IsValid())
  {
    ret += i.at_class();
    i.Increment();
  }
  return ret;
}

void TrainRoutine(DataSelector d_s, MyIterator<MyNode> t_i, std::vector<int> include, RandomUniform &r, MyTree<MyNode> &tree) {
  int num = d_s.Length();
  if (num < 2)
  {
    return;
  }
  if (t_i.GetRemainingDepth() == 1) {
    std::cout << "[NOTICE] Reached final leaf" << std::endl;
    //Ищем к какому классу относится большинство элементов, которые в d_s и этот класс и присваиваем
    int num = d_s.Length();
    int in_class_1 = CountClass(d_s, tree);
    if(in_class_1 > num / 2) {
      t_i.el().u.classification = 1;
    } else {
      t_i.el().u.classification = 0;
    }
  } else {
    int feature = r.GetRandFromVectorAndErase(include);
    std::cout << "[NOTICE] Starting new leaf. Feature = " << feature <<  std::endl;
    int treshold = FindOptimalTreshold(d_s, feature);
    std::cout << "[NOTICE] Treshold = " << feature <<  std::endl;
    t_i.el().feature_num = feature;
    t_i.el().u.treshold = treshold;
    //Left:
    {
      auto d_s_l = d_s;
      auto t_i_l = t_i;
      d_s_l.Unselect(t_i.el(), true);
      t_i_l.Move(true);
      TrainRoutine(d_s_l, t_i_l, include, r, tree);
    }
    //Right:
    {
      auto d_s_l = d_s;
      auto t_i_l = t_i;
      d_s_l.Unselect(t_i.el(), false);
      t_i_l.Move(false);
      TrainRoutine(d_s_l, t_i_l, include, r, tree);
    }
  }
}

int CheckData(MyTree<MyNode> &tree, const Data &d) {
  int right_ans = 0;
  for(int i = 0; i < d.x.size(); i++)
  {
    if(Classify(d.x[i], tree) == d.y[i])
      right_ans++;
  }
  return right_ans;
}

int main() {
  Data data("data.csv", TRAIN_SIZE, NUM_FEATURES);
  Data test("test.csv", TEST_SIZE, NUM_FEATURES);
  MyTree<MyNode> tree(3);
  std::vector<int> include = {0, 1}; // Столбцы для тренировки
  RandomUniform r;
  std::cout << "[NOTICE] Strating training" << std::endl;
  TrainRoutine(DataSelector(TRAIN_SIZE, data), MyIterator<MyNode>(tree), include, r, tree);
  std::cout << "[NOTICE] " << CheckData(tree, test) << " right answers out of " << TEST_SIZE << std::endl;
  return 0;
}
