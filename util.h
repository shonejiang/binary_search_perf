#pragma once

#include <chrono>
#include <random>
#include <iostream>
#include <vector>
#include <algorithm> // for std::sort (if needed for testing)

// for SIMD intrinsics
#ifdef __AVX2__
#include <immintrin.h> // For AVX2 (256-bit SIMD)
#elif __SSE2__
#include <emmintrin.h> // For SSE2 (128-bit SIMD)
#endif
// 使用 GCC/Clang 特有的 __attribute__((always_inline))
// 对于 MSVC，可以使用 __forceinline



#pragma pack(push, 1)
struct PV {
  long long int prc;
  int32_t size;
  int32_t cnt;
};
#pragma pack(pop)

static_assert(sizeof(PV) == 16, "PV size is not 16");

class BiasedRandomGenerator {
private:
    std::mt19937 gen;
    std::uniform_real_distribution<> prob_dis;  // [0, 1) 概率判断
    std::uniform_int_distribution<> front_dis;  // 前 (1-threshold)% 区间
    std::uniform_int_distribution<> back_dis;   // 后 threshold% 区间
    double threshold_prob_; // 高概率出现的区间对应的概率
    const int back_range_start_; // 后 threshold% 区间的起始索引
    
public:
    BiasedRandomGenerator(int size, double high_prob_in_back_range_percentage) 
        : gen(std::random_device{}()),
          prob_dis(0.0, 1.0),
          threshold_prob_(high_prob_in_back_range_percentage), // 比如 0.83
          back_range_start_(static_cast<int>(size * (1.0 - high_prob_in_back_range_percentage))) // 1 - 0.83 = 0.17 -> 17% 开始
    {
        // 确保区间正确，并处理 size 过小的情况
        if (size <= 0) {
            front_dis = std::uniform_int_distribution<>(0, 0); // 或者抛出异常
            back_dis = std::uniform_int_distribution<>(0, 0);
            return;
        }
        
        const int actual_back_range_start = static_cast<int>(size * (1.0 - high_prob_in_back_range_percentage));
        const int concentration_start_idx = static_cast<int>(size * (1.0 - high_prob_in_back_range_percentage));

        const int back_region_start = static_cast<int>(size * 0.83); // Start of the last 17% of the array
        const int front_region_end = back_region_start - 1; // End of the first 83% of the array


        if (back_region_start >= size) { // If the "back region" effectively covers the whole array or is invalid
             front_dis = std::uniform_int_distribution<>(0, size - 1); // Only front range
             back_dis = std::uniform_int_distribution<>(0, size - 1);
        } else if (front_region_end < 0) { // If the "front region" is empty or invalid
            front_dis = std::uniform_int_distribution<>(back_region_start, size - 1); // Only back range
            back_dis = std::uniform_int_distribution<>(back_region_start, size - 1);
        } else {
            front_dis = std::uniform_int_distribution<>(0, front_region_end);
            back_dis = std::uniform_int_distribution<>(back_region_start, size - 1);
        }
    }
    
    int generate() {
        if (prob_dis(gen) < threshold_prob_) { // e.g., 83% probability
            return back_dis(gen);
        } else { // e.g., 17% probability
            return front_dis(gen);
        }
    }
};



template<typename T>
void perf_test(std::string_view name, const int query_cnt, BiasedRandomGenerator& biased_gen, std::vector<PV>& arr, T func) {
  uint64_t total_nanoseconds = 0;
  for (int i = 0; i < query_cnt; i++) {
    auto target = biased_gen.generate();
    auto start = std::chrono::high_resolution_clock::now();
    volatile int a = func(arr, target); // 使用 volatile 避免优化掉整个查找
    (void)a;
    auto end = std::chrono::high_resolution_clock::now();
    total_nanoseconds += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  }
  
  std::cout << name << ": \t\t " 
  << static_cast<double>(total_nanoseconds) / query_cnt << " ns/query\n";
}
