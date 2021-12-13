#include "Data.h"

#include <iostream>
#include <fstream>

#include <omp.h>

RawData::RawData(const std::string &txt) {
  int num_entries;
  std::ifstream f(txt);
  if (!f.is_open()) {
    std::cout << "[ERROR] Couldn't open input file" << std::endl;
    exit(-1);
  }
  std::cout << "[NOTICE] Starting reading. Progress:          end \\/" << std::endl;
  f >> articles >> num_entries >> num_entries;
  d = std::vector<std::vector<int>>(articles);
  int current_article = 0;
  for (int i = 0; i < num_entries; i++) {
    if (i % (num_entries / 50) == 0) {
      std::cout << "-";
    }
    int a, b, c;
    f >> a >> b >> c;
    if (a - 1 != current_article) {
      current_article = a - 1;
      d[current_article] = {};
    }
    d[current_article].emplace_back(b);
  }
  f.close();
  std::cout << "\n[NOTICE] Read \"" << txt << "\". Total " << num_entries << " entries." << std::endl;
}

int RawData::Distance(int a, int b) const {
  auto i = d[a].begin(), j = d[b].begin();
  int res = 0;
  while(true) {
    if (i == d[a].end()) {
      while (j != d[b].end()) {
        j++;
        res++;
      }
      break;
    }
    if (j == d[b].end()) {
      while (i != d[a].end()) {
        i++;
        res++;
      }
      break;
    }
    if (*i == *j) {
      i++;
      j++;
    } else if (*i < *j) {
      i++;
      res++;
    } else {
      j++;
      res++;
    }
  }
  return res;
}

Distances::Distances(const RawData &r) {
  articles = r.articles;
  d = std::vector<std::vector<int>>(articles);
  std::cout << "[NOTICE] Calculating distances. Progress:     end \\/" << std::endl;
  for (int i = 0; i < articles; i++) {
    d[i] = std::vector<int>(articles - i);
    d[i][0] = 0;
  }
  for (int i = 0; i < articles; i++) {
    if (i % (articles/50) == 0) {
      std::cout << "-";
    }
    #pragma omp parallel for
    for (int j = i + 1; j < articles; j++) {
      int n_d = r.Distance(i, j);
      d[i][j-i] = n_d;
    }
  }
  std::cout << "\n[NOTICE] Calculated " << articles*articles << " distances." << std::endl;
}

int Distances::At(int i, int j) const {
  return d[std::max(i, j)][std::min(i, j) - i];
}

void Distances::BarChartOut(const std::string &out, int max) const {
  std::vector<int> v(max+1, 0);
  std::cout << "[NOTICE] Writing out bar chart. Progress:     end \\/" << std::endl;
  for (int i = 0; i < articles; i++) {
    if (i % (articles/50) == 0) {
      std::cout << "-";
    }
    for (int j = i + 1; j < articles; j++) {
      if (this->At(i, j) >= max) {
        v[max]++;
      } else {
        v[this->At(i, j)]++;
      }
    }
  }
  std::ofstream f(out);
  if (!f.is_open()) {
    std::cout << "[ERROR] Couldn't open output file" << std::endl;
    exit(-1);
  }
  for (int i = 0; i < max + 1; i++) {
    f << v[i] << " ";
  }
  std::cout << "\n[NOTICE] Wrote out bar chart. With threshold = " << max << "." << std::endl;
}

void Distances::ToFile(const std::string &s) const {
  std::cout << "[NOTICE] Outputting distances to a file \"" << s << "\"\n";
  std::cout << "[NOTICE] Progress                             end \\/" << std::endl;
  std::ofstream f(s);
  f << articles << "\n";
  for (int i = 0; i < articles; i++) {
    if (i % (articles/50) == 0) {
      std::cout << "-";
    }
    for (int j = i + 1; j < articles; j++) {
      f << d[i][j-i] << " ";
    }
  }
  f.close();
  std::cout << "\n[NOTICE] Success" << std::endl;
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

bool Equal(const Distances &a, const Distances &b) {
  std::cout << "[NOTICE] Testing distances equality." << std::endl;
  if (a.Articles() != b.Articles()) {
    return false;
  }
  std::cout << "[NOTICE] Progress:                            end \\/" << std::endl;
  for (int i = 0; i < a.Articles(); i++) {
    if (i % (a.Articles()/50) == 0) {
      std::cout << "-";
    }
    for (int j = i + 1; j < a.Articles(); j++) {
      if (a.At(i, j) != b.At(i, j)) {
        return false;
      }
    }
  }
  std::cout << "\n[NOTICE] Done! Equal" << std::endl;
  return true;
}