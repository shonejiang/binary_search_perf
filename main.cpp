#include "util.h"
#include "algo.h"

int main() {
  const int LEN = 1024;
  const int QUERY_CNT = 1000000;
  std::vector<PV> arr(LEN);
  for (int i = 0; i < LEN; i++) arr[i].prc = i; // 0, 1, ..., LEN-1
  
  // 0.83 表示 83% 的概率生成位于数组后 17% 区间的值
  double concentration_probability = 0.83; 
  BiasedRandomGenerator biased_gen(LEN, concentration_probability);
  
  std::cout << "Running benchmark with LEN = " << LEN << ", Concentration probability in last 17% of array: " 
            << concentration_probability * 100 << "%" << std::endl << std::endl;


  perf_test("stl", QUERY_CNT, biased_gen, arr, binary_search_stl);
  std::cout << "--------------------------------------------------------------------------" << std::endl;

  perf_test("branch", QUERY_CNT, biased_gen, arr, binary_search_branch);
  perf_test("  branchless", QUERY_CNT, biased_gen, arr, binary_search_branchless);
  perf_test("  branchless1", QUERY_CNT, biased_gen, arr, binary_search_branchless1);
  perf_test("  branchless2_0", QUERY_CNT, biased_gen, arr, binary_search_branchless2_0);
  perf_test("  branchless2_1", QUERY_CNT, biased_gen, arr, binary_search_branchless2_1);
  perf_test("  branchless2_2", QUERY_CNT, biased_gen, arr, binary_search_branchless2_2);
  perf_test("  branchless2_3", QUERY_CNT, biased_gen, arr, binary_search_branchless2_3);
  perf_test("  branchless3", QUERY_CNT, biased_gen, arr, binary_search_branchless3);
  perf_test("  branchless4", QUERY_CNT, biased_gen, arr, binary_search_branchless4);
  perf_test("  branchless5", QUERY_CNT, biased_gen, arr, binary_search_branchless5);
  perf_test("  75percent", QUERY_CNT, biased_gen, arr, binary_search_75percent);
  perf_test("  83percent", QUERY_CNT, biased_gen, arr, binary_search_83percent);
  std::cout << "--------------------------------------------------------------------------" << std::endl;

  perf_test("liner", QUERY_CNT, biased_gen, arr, binary_search_liner);
  perf_test("  reverse", QUERY_CNT, biased_gen, arr, binary_search_reverse_liner);
  perf_test("  pragma", QUERY_CNT, biased_gen, arr, binary_search_pragma);
  perf_test("  unloop", QUERY_CNT, biased_gen, arr, binary_search_unloop);
  perf_test("  simd", LEN, biased_gen, arr, binary_search_simd);
  perf_test("  simd_gather", LEN, biased_gen, arr, binary_search_simd_gather);
}
