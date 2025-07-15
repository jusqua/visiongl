#include <chrono>
#include <functional>
#include <string>
#include <vector>
#include <iostream>

struct BenchmarkSpec {
  std::string name;
  std::function<void(void)> func;
  std::function<void(void)> post;
};

class BenchmarkBuilder {
private:
    std::vector<BenchmarkSpec> specs;

    void measure_time(std::size_t rounds, const BenchmarkSpec &spec) {
        std::cout << spec.name << ": ";

        auto time_start_once = std::chrono::high_resolution_clock::now();
        spec.func();
        auto time_end_once = std::chrono::high_resolution_clock::now();
        double once_duration = std::chrono::duration<double>(time_end_once - time_start_once).count();

        std::cout << once_duration << "s (once)";

        if (rounds <= 1) {
            std::cout << std::endl;
            spec.post();
            return;
        }

        auto time_start_times = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < rounds; ++i) {
            spec.func();
        }
        auto time_end_times = std::chrono::high_resolution_clock::now();

        double times_duration = std::chrono::duration<double>(time_end_times - time_start_times).count();
        std::cout << " | " << times_duration << "s (" << rounds << " times)" << std::endl;
        spec.post();
    }

public:
    inline void attach(BenchmarkSpec spec) { specs.push_back(spec); }

    inline void run(std::size_t rounds) {
        for (const auto& spec : specs) {
            measure_time(rounds, spec);
        }
    }
};
