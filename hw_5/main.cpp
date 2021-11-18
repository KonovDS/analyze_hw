#include <iostream>
#include <vector>
#include <utility> // std::pair
#include <random>
#include <cmath>
#include <fstream>
#include <algorithm>

#define EPSILON 1e-6

inline double sign(double x) {
  return x<0?-1.0:1.0;
}

class Data {
  std::vector<std::pair<double, int>> d;
 public:
  Data() = default;

  int Size() const {
    return d.size();
  }

  const std::pair<double, int> &operator[](int index) const {
    return d[index];
  }

  void GenerateTriangle(double from, double to, int type, int num_entries) {
    std::random_device rd;
    std::mt19937 gen = std::mt19937(rd());
    auto distrib_x = std::uniform_real_distribution<>(0, std::abs(from - to));
    for (int i = 0; i < num_entries; i++) {
      double x_coords = from + sign(to - from) * std::max(distrib_x(gen), distrib_x(gen));
      d.emplace_back(x_coords, type);
    }
  }

  void Sort() {
    std::sort(d.begin(), d.end(), [](auto a, auto b){ return a.first > b.first; });
  }

};

struct ConfusionMatrix {
  int t_p, f_p, f_n, t_n;
};

double Accuracy(const ConfusionMatrix &c_m) {
  return static_cast<double>(c_m.t_p + c_m.t_n) / (c_m.t_p + c_m.t_n + c_m.f_p + c_m.f_n);
}

// i.e Sensitivity
double Recall(const ConfusionMatrix &c_m) {
  return static_cast<double>(c_m.t_p) / (c_m.t_p + c_m.f_n);
}

double Specificity(const ConfusionMatrix &c_m) {
  return static_cast<double>(c_m.t_n) / (c_m.t_n + c_m.f_p);
}

double Precision(const ConfusionMatrix &c_m) {
  return static_cast<double>(c_m.t_p) / (c_m.t_p + c_m.f_p);
}

double F1Score(const ConfusionMatrix &c_m) {
  double prec = Precision(c_m);
  double rec = Recall(c_m);
  return 2*prec*rec/(prec + rec);
}

class Predictor {
  double treshold;
 public:
  explicit Predictor(double t) : treshold(t) {}

  int Predict(double x) const {
    return x<treshold?0:1;
  }

  struct ConfusionMatrix ConfusionMatrix(const Data &d) const {
    struct ConfusionMatrix ret({0,0,0,0});
    int n = d.Size();
    for (int i = 0; i < n; i++) {
      if (Predict(d[i].first) == 0) {
        if (d[i].second == 0) {
          ret.t_n++;
        } else {
          ret.f_n++;
        }
      } else {
        if (d[i].second == 1) {
          ret.t_p++;
        } else {
          ret.f_p++;
        }
      }
    }
    return ret;
  }

  int FalsePosititves(const Data& d) const {
    int ret = 0;
    for(int i = 0; i < d.Size(); i++) {
      if (d[i].second == 0 && Predict(d[i].first) == 1) {
        ret++;
      }
    }
    return ret;
  }

  int TruePositives(const Data& d) const {
    int ret = 0;
    for(int i = 0; i < d.Size(); i++) {
      if (d[i].second == 1 && Predict(d[i].first) == 1) {
        ret++;
      }
    }
    return ret;
  }
};

void WriteOutExperimentResults(const std::string &s, double from, double to, double step, const Data &d) {
  std::ofstream fs;
  fs.open(s);
  if (!fs.is_open()) {
    std::cout << "[ERROR] Unable to open output file." << std::endl;
    exit(-1);
  }
  fs << "threshold,specificity,accuracy,precision,recall,f1score\n";
  for (; from <= to + EPSILON; from += step) {
    Predictor p(from);
    auto c_m = p.ConfusionMatrix(d);
    fs << from << "," << Specificity(c_m) << ","
       << Accuracy(c_m) << "," << Precision(c_m) << ","
       << Recall(c_m) << "," << F1Score(c_m) << "\n";
  }
  fs.close();
}

void ROCAUCGraphWriteOut(const std::string &s, const Data &d, const Predictor &p, int pos, int neg) {
  std::ofstream fs;
  fs.open(s);
  if (!fs.is_open()) {
    std::cout << "[ERROR] Unable to open output file." << std::endl;
    exit(-1);
  }
  int x = 0, y = 0;
  fs << "False Positive, True Positive\n";
  for (int i = 0; i < d.Size(); i++) {
    if (d[i].second == 0 && p.Predict(d[i].first) == 1) {
      y++;
    } else if (d[i].second == 1 && p.Predict(d[i].first) == 1) {
      x++;
    } else {
      continue;
    }
    fs << static_cast<double>(x)/pos << "," << static_cast<double>(y)/neg << "\n";
  }
  fs.close();
}

int main() {
  Data d;
  d.GenerateTriangle(0, 1, 1, 10000);
  d.GenerateTriangle(1, 0, 0, 10000);
  WriteOutExperimentResults("exp1.csv", 0.0, 1.0, 0.01, d);
  {
    Data d_1; // Дополнительный эксперимент с разреженными данными
    d_1.GenerateTriangle(0, 1, 1, 1000);
    d_1.GenerateTriangle(1, 0, 0, 19000);
    WriteOutExperimentResults("exp2.csv", 0.0, 1.0, 0.01, d_1);
  }
  d.Sort();
  Predictor p = Predictor(0.5);
  ROCAUCGraphWriteOut("exp3.csv", d, p, p.TruePositives(d), p.FalsePosititves(d));
  //PRGraphWriteOut("exp4.csv", d, p, p.TruePositives(d), p.FalsePosititves(d));
  return 0;
}
