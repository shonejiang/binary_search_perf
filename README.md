```
g++ main.cpp -std=c++23 -O3 -march=native && taskset -c 31 ./a.out 
Running benchmark with LEN = 1024, Concentration probability in last 17% of array: 83%

stl:             46.0682 ns/query
--------------------------------------------------------------------------
branch:                  46.0679 ns/query
  branchless:            46.1897 ns/query
  branchless1:           49.2279 ns/query
  branchless2_1:                 38.4247 ns/query
  branchless2_2:                 38.1099 ns/query
  branchless2_3:                 27.0639 ns/query
  branchless3:           51.229 ns/query
  branchless4:           50.9194 ns/query
  branchless5:           55.2687 ns/query
  75percent:             41.0203 ns/query
  83percent:             44.5402 ns/query
--------------------------------------------------------------------------
liner:           42.3678 ns/query
  reverse:               41.1096 ns/query
  pragma:                42.3858 ns/query
  unloop:                41.6473 ns/query
  simd:                  42.4043 ns/query
  simd_gather:           89.0645 ns/query
```