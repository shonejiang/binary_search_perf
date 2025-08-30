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
  perf_test("base", QUERY_CNT, biased_gen, arr, binary_search_branch);
  std::cout << "--------------------------------------------------------------------------" << std::endl;

  perf_test("bit_oper", QUERY_CNT, biased_gen, arr, binary_search_bit_op);
  perf_test("unlikely", QUERY_CNT, biased_gen, arr, binary_search_unlikely);
  perf_test("ternary", QUERY_CNT, biased_gen, arr, binary_search_ternary);
  perf_test("array", QUERY_CNT, biased_gen, arr, binary_search_array);
  perf_test("branchless_mask", QUERY_CNT, biased_gen, arr, binary_search_branchless_mask);
  perf_test("75percent", QUERY_CNT, biased_gen, arr, binary_search_75percent);
  std::cout << "--------------------------------------------------------------------------" << std::endl;

  perf_test("less_instruction_branchless", QUERY_CNT, biased_gen, arr, less_instruction_branchless_return_at_end);
  perf_test("  midway", QUERY_CNT, biased_gen, arr, less_instruction_branchless_return_midway);
  perf_test("  likely", QUERY_CNT, biased_gen, arr, less_instruction_branchless_likely);
  perf_test("  75p_and_end", QUERY_CNT, biased_gen, arr, less_instruction_branchless_75p_and_return_at_end);
  perf_test("  75p_and_mid", QUERY_CNT, biased_gen, arr, less_instruction_branchless_75p_and_return_at_mid);
  std::cout << "--------------------------------------------------------------------------" << std::endl;

  perf_test("liner", QUERY_CNT, biased_gen, arr, binary_search_liner);
  perf_test("  liner_and_75p", QUERY_CNT, biased_gen, arr, binary_search_liner_75);
  perf_test("  reverse", QUERY_CNT, biased_gen, arr, binary_search_reverse_liner);
  perf_test("  pragma", QUERY_CNT, biased_gen, arr, binary_search_pragma);
  perf_test("  unloop", QUERY_CNT, biased_gen, arr, binary_search_unloop);
  perf_test("  simd", LEN, biased_gen, arr, binary_search_simd);
  perf_test("  simd_gather", LEN, biased_gen, arr, binary_search_simd_gather);
  std::cout << "--------------------------------------------------------------------------" << std::endl;

  perf_test("bit_75p_early_ret_linner_scan", QUERY_CNT, biased_gen, arr, bit_75p_early_ret_linner_scan);
}
