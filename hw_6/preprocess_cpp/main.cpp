#include <iostream>

#include "data.h"

void Routine(const std::string &in, const std::string &out, const std::string &bar) {
  std::cout << "[NOTICE] Converting file \"" << in
  << "\" to distance file \"" << out << "\"."
  << std::endl;

  RawData data_1(in);
  Distances dist_1(data_1);
  dist_1.ToFile(out);
  dist_1.BarChartOut(bar, 1298);
  // Проверка файла:
  Distances dist_1_1(out);
  if (!Equal(dist_1, dist_1_1)) {
    std::cout << "\n[ERROR] Error. Wrong output file data" << std::endl;
    exit(-1);
  }
}

int main() {
  std::string path_1 = "docword.kos.txt";
  std::string out_1 = "dist.kos.txt";
  std::string bar_1 = "out_1.txt";

  std::string path_2 = "docword.nips.txt";
  std::string out_2 = "dist.nips.txt";
  std::string bar_2 = "out_2.txt";

  Routine(path_1, out_1, bar_1);
  Routine(path_2, out_2, bar_2);

  std::cout << "[SUCCESS] All output files tested successfully. Exiting" << std::endl;
  return 0;
}