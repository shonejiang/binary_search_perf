```
g++ main.cpp -std=c++23 -O3 -march=native && taskset -c 31 ./a.out 
Running benchmark with LEN = 1024, Concentration probability in last 17% of array: 83%

stl:             46.0948 ns/query
base:            49.3517 ns/query
--------------------------------------------------------------------------
bit_oper:                49.025 ns/query
unlikely:                49.3892 ns/query
ternary:                 48.7408 ns/query
array:           49.3734 ns/query
branchless_mask:                 52.9668 ns/query
75percent:               45.0156 ns/query
--------------------------------------------------------------------------
less_instruction_branchless:             37.9983 ns/query
  midway:                35.9809 ns/query
  likely:                37.9705 ns/query
  75p_and_end:           27.2849 ns/query
  75p_and_mid:           26.7037 ns/query
--------------------------------------------------------------------------
liner:           39.9492 ns/query
  liner_and_75p:                 40.0007 ns/query
  reverse:               39.8164 ns/query
  pragma:                40.1097 ns/query
  unloop:                40.2502 ns/query
  simd:                  43.1572 ns/query
  simd_gather:           66.0791 ns/query
--------------------------------------------------------------------------
bit_75p_early_ret_linner_scan:           25.2576 ns/query
```