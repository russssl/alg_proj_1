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

const std::string filename = "shell_sort_results.csv";

void saveResult(int n, double time) {
  bool need_header = false;
  {
    std::ifstream ifs(filename);
    if (!ifs.good()) {
      need_header = true;
    } else {
      ifs.seekg(0, std::ios::end);
      if (ifs.tellg() == 0) need_header = true;
    }
  }

  std::ofstream ofs(filename, std::ios::app);
  if (!ofs) {
    std::cerr << "Failed to open " << filename << " for writing\n";
    return;
  }

  if (need_header) ofs << "n,time(s)\n";
  ofs << n << ',' << std::fixed << std::setprecision(6) << time << ',' << '\n';
}

void prepareFile() {
  std::ofstream ofs(filename, std::ios::trunc);
  if (!ofs) {
    std::cerr << "Failed to open " << filename << " for writing\n";
    return;
  }
  ofs << "n,time(s)\n";
}

int main() {
  const int array_size = 50000;
  TimeCounter t;

  // create RNG once for reproducibility and speed
  std::random_device rd;
  std::mt19937 gen(rd());

  prepareFile();

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

    saveResult(n, sortingTime);
    delete[] arr;
  }
  return 0;
}
