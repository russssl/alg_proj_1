#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

class TimeCounter {
private:
  using clock = chrono::high_resolution_clock;
  chrono::time_point<clock> start_time{};
  chrono::time_point<clock> end_time{};
public:

  TimeCounter() = default;

  void start() { start_time = clock::now(); }
  void stop() { end_time = clock::now(); }
  double get_time() const {
    return chrono::duration<double>(end_time - start_time).count();
  }
};

void randomizeArray(int* arr, int n, std::mt19937 &gen) {
  if (n <= 0 || arr == nullptr) return;
  std::uniform_int_distribution<int> dist(0, 1'000'000);
  for (int i = 0; i < n; ++i) arr[i] = dist(gen);
}

void shellSort(int* arr, int n, const int* gaps, int gaps_count) {
  if (n <= 1 || arr == nullptr || gaps == nullptr || gaps_count <= 0) return;
  for (int gi = 0; gi < gaps_count; ++gi) {
    int gap = gaps[gi];
    if (gap <= 0 || gap >= n) continue;
    for (int i = gap; i < n; ++i) {
      int temp = arr[i];
      int j = i;
      while (j >= gap && arr[j - gap] > temp) {
        arr[j] = arr[j - gap];
        j -= gap;
      }
      arr[j] = temp;
    }
  }
}
void saveResult(int n, double time, const std::string &filename) {
  bool need_header = false;
  {
    std::ifstream ifs(filename);
    if (!ifs) {
      need_header = true;
    } else {
      ifs.seekg(0, std::ios::end);
      if (ifs.tellg() == 0) need_header = true;
    }
  }

  std::ofstream ofs(filename, std::ios::app);
  if (!ofs) {
    std::ofstream create(filename);
    if (!create) {
      std::cerr << "Failed to create " << filename << " for writing\n";
      return;
    }
    create.close();
    ofs.open(filename, std::ios::app);
    if (!ofs) {
      std::cerr << "Failed to open " << filename << " for writing\n";
      return;
    }
  }

  if (need_header) ofs << "n,time(s)\n";
  ofs << n << ',' << std::fixed << std::setprecision(6) << time << '\n';
}

void saveCombined(int n, const std::vector<double> &times, const std::string &labels, const std::string &filename) {
  bool need_header = false;
  {
    std::ifstream ifs(filename);
    if (!ifs) {
      need_header = true;
    } else {
      ifs.seekg(0, std::ios::end);
      if (ifs.tellg() == 0) need_header = true;
    }
  }

  std::ofstream ofs(filename, std::ios::app);
  if (!ofs) {
    std::ofstream create(filename);
    if (!create) {
      std::cerr << "Failed to create " << filename << " for writing\n";
      return;
    }
    create.close();
    ofs.open(filename, std::ios::app);
    if (!ofs) {
      std::cerr << "Failed to open " << filename << " for writing\n";
      return;
    }
  }

  if (need_header) {
    ofs << "n";
    for (const auto &lab : labels) ofs << ',' << lab;
    ofs << '\n';
  }

  ofs << n;
  for (double t : times) ofs << ',' << std::fixed << std::setprecision(6) << t;
  ofs << '\n';
}

void firstTest() {
  const std::string test_1_file = "test_1.csv";
  const int array_size = 50000;
  TimeCounter t;

  // create RNG once for reproducibility and speed
  std::random_device rd;
  std::mt19937 gen(rd());


  for (int n = 419; n >= 2; n--) {
    int* arr = new int[array_size];

    // fill array using shared RNG
    randomizeArray(arr, array_size, gen);

    // we want to test gaps {420, n, 1}
    int gaps[3] = {420, n, 1};

    // measure only the sorting time with a fresh timer
    TimeCounter sortTimer;
    sortTimer.start();
    shellSort(arr, array_size, gaps, 3);
    sortTimer.stop();

    double sortingTime = sortTimer.get_time();


    cout << "Czas sortowania (Shell, kroki 420, " << n << ", 1): " << sortingTime << " s";

    saveResult(n, sortingTime, test_1_file);
    delete[] arr;
  }
}

double singleTest(const int* seq, int seq_len, int arr_size, std::mt19937 &gen) {
  int* arr = new int[arr_size];
  randomizeArray(arr, arr_size, gen);
  TimeCounter sortTimer;
  sortTimer.start();
  shellSort(arr, arr_size, seq, seq_len);
  sortTimer.stop();

  double sortingTime = sortTimer.get_time();
  delete[] arr;
  return sortingTime;
}

void secondTest() {
  const int knuth[] = {88573, 29524, 9841, 3280, 1093, 364, 121, 40, 13, 4, 1};

  const int sedgewick[] = {71065, 36289, 16001, 8929, 3905, 2161, 929, 505, 209, 109, 41, 19, 5, 1};

  const int mine_1[] = {89321, 67259, 58123, 49877, 42109, 37421, 29571, 24359, 19733, 15487, 12011, 9013, 7021, 5039, 3571, 2537, 1597, 997, 521, 271, 137, 71, 37, 17, 7, 3, 1};

  const int mine_2[] = {80000, 48000, 36000, 27000, 20000, 15000, 10000, 7000, 5000, 3500, 2000, 1200, 700, 350, 200, 110, 50, 25, 10, 5, 1};

  const int knuth_count = sizeof(knuth) / sizeof(knuth[0]);
  const int sedgewick_count = sizeof(sedgewick) / sizeof(sedgewick[0]);
  const int mine_1_count = sizeof(mine_1) / sizeof(mine_1[0]);
  const int mine_2_count = sizeof(mine_2) / sizeof(mine_2[0]);

  std::random_device rd;
  std::mt19937 gen(rd());

  // prepare combined CSV file for easy plotting
  const std::string combined_file = "test_2_combined.csv";
  // const std::vector<std::string> labels = {"knuth", "sedgewick", "mine_1", "mine_2"};
  const std::string labels[4] = {"knuth", "sedgewick", "mine_1", "mine_2"};
  {
    std::ofstream f(combined_file, std::ios::trunc);
    if (f) {
      f << "n";
      for (const auto &lab : labels) f << ',' << lab;
      f << '\n';
    }
  }

  for (int size = 2000; size <= 100000; size += 2000) {
    double t_knuth = singleTest(knuth, knuth_count, size, gen);
    double t_sedge = singleTest(sedgewick, sedgewick_count, size, gen);
    double t_m1 = singleTest(mine_1, mine_1_count, size, gen);
    double t_m2 = singleTest(mine_2, mine_2_count, size, gen);

    std::vector<double> times = {t_knuth, t_sedge, t_m1, t_m2};
    saveCombined(size, times, labels, combined_file);
  }
}
int main() {
  secondTest();
  return 0;
}
