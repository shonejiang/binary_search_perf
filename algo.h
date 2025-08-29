#include "util.h"
#pragma once

#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif


__always_inline int binary_search_stl(std::vector<PV>& arr, int target) noexcept {
  auto it = std::lower_bound(arr.begin(), arr.end(), target, [](const PV& a, int b) { return a.prc < b; });
  if (it != arr.end() && it->prc == target) return it - arr.begin();
  return -1;
}

__always_inline int binary_search_branch(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left <= right) {
      int mid = left + ((right - left) >> 1);

        if (arr[mid].prc == target) return mid;

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }
    return -1;
}

// 修正后的无分支版本
__always_inline int binary_search_branchless(std::vector<PV>& arr, int target) noexcept {
  int left = 0, right = arr.size() - 1;
  while (left <= right) {
      int mid = left + ((right - left) >> 1);
      if (arr[mid].prc == target) return mid; // 等于的判断通常保留
      
      int next_left_val = mid + 1;
      int next_right_val = mid - 1;

      left = (arr[mid].prc < target) ? next_left_val : left;
      right = (arr[mid].prc < target) ? right : next_right_val;
  }
  return -1;
}

__always_inline int binary_search_branchless1(std::vector<PV>& arr, int target) noexcept {
  int left = 0, right = arr.size() - 1;
  while (left < right) {  // 注意: < 而不是 <=
    int mid = left + ((right - left) >> 1);
    
    // 核心技巧：使用位运算避免分支
    // 如果 arr[mid].prc < target, mask = -1 (all 1s)
    // 否则 mask = 0
    int mask = -(arr[mid].prc < target);
    
    // branchless 更新
    left = (mask & (mid + 1)) | (~mask & left);
    right = (~mask & (mid - 1)) | (mask & right);
  }

  return (arr[left].prc == target) ? left : -1;
}

__always_inline int binary_search_branchless2_0(std::vector<PV>& arr, int target) noexcept {
  int size = arr.size();
  int left = 0;
  int len = size;
  
  // 展开循环，每次确定一半
  while (len > 1) {
      int half = len >> 1;
      int mid = left + half;
      
      if (arr[mid].prc == target) return mid;
      // 使用 __builtin_expect 提示
      left = arr[mid].prc < target ? mid : left;
      len -= half;
  }
  
  return (left < size && arr[left].prc == target) ? left : -1;
}
__always_inline int binary_search_branchless2_1(std::vector<PV>& arr, int target) noexcept {
  int size = arr.size();
  int left = 0;
  int len = size;
  
  // 展开循环，每次确定一半
  while (len > 1) {
      int half = len >> 1;
      int mid = left + half;
      
      // 使用 __builtin_expect 提示
      left = arr[mid].prc < target ? mid : left;
      len -= half;
  }
  
  return (left < size && arr[left].prc == target) ? left : -1;
}
__always_inline int binary_search_branchless2_2(std::vector<PV>& arr, int target) noexcept {
  int size = arr.size();
  int left = 0;
  int len = size;
  
  // 展开循环，每次确定一半
  while (len > 1) {
      int half = len >> 1;
      int mid = left + half;
      
      // 使用 __builtin_expect 提示
      left = __builtin_expect(arr[mid].prc < target, 1) ? mid : left;
      len -= half;
  }
  
  return (left < size && arr[left].prc == target) ? left : -1;
}

__always_inline int binary_search_branchless2_3(std::vector<PV>& arr, int target) noexcept {
  int size = arr.size();
  int left = 0;
  int len = size;
  
  // 展开循环，每次确定一半
  while (len > 1) {
      int half = (len >> 1) + (len >> 2);
      int mid = left + half;
      
      // 使用 __builtin_expect 提示
      left = arr[mid].prc < target ? mid : left;
      len -= half;
  }
  
  return (left < size && arr[left].prc == target) ? left : -1;
}

__always_inline int binary_search_branchless3(std::vector<PV>& arr, int target) noexcept {
  int left = 0;
  int right = arr.size() - 1;
  
  // 每次将搜索范围缩小一半
  while (left < right) {
      int mid = left + ((right - left + 1) >> 1);  // 注意: +1 保证向上取整
      
      // 关键：根据比较结果移动 left 或保持不变
      // 如果 arr[mid] <= target，说明目标在 [mid, right]
      // 否则目标在 [left, mid-1]
      left = __builtin_expect(arr[mid].prc <= target, 1) ? mid : left;
      right = __builtin_expect(arr[mid].prc <= target, 1) ? right : mid - 1;
  }
  
  return (left < arr.size() && arr[left].prc == target) ? left : -1;
}

__always_inline int binary_search_branchless4(std::vector<PV>& arr, int target) noexcept {
  int left = 0;
  int right = arr.size() - 1;
  
  while (left < right) {
      int mid = left + ((right - left + 1) >> 1);
      
      // 使用条件移动而不是三元运算符
      // 这种写法更容易生成 cmov 指令
      if (__builtin_expect(arr[mid].prc <= target, 1)) {
          left = mid;  // 目标在右半部分或就是 mid
      } else {
          right = mid - 1;  // 目标在左半部分
      }
  }
  
  return (arr[left].prc == target) ? left : -1;
}

__always_inline int binary_search_branchless5(std::vector<PV>& arr, int target) noexcept {
  int left = 0;
  int right = arr.size() - 1;
  
  while (left < right) {
      int mid = left + ((right - left + 1) >> 1);
      
      // 创建掩码：如果 arr[mid].prc <= target，mask = -1，否则 mask = 0
      int cond = arr[mid].prc <= target;
      int mask = -cond;  // 0 或 -1
      
      // branchless 更新
      left = (mask & mid) | (~mask & left);
      right = (~mask & (mid - 1)) | (mask & right);
  }
  
  return (arr[left].prc == target) ? left : -1;
}

__always_inline int binary_search_75percent(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left <= right) {
      int mid = right - ((right - left) >> 2);

        if (arr[mid].prc == target) return mid;

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }
    return -1;
}

__always_inline int binary_search_83percent(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left <= right) {
      int mid = right - ((right - left) >> 3) - ((right - left) >> 4);

        if (arr[mid].prc == target) return mid;

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }
    return -1;
}

__always_inline int binary_search_liner(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left <= right) {
      int mid = left + ((right - left) >> 1);

        if (arr[mid].prc == target) return mid;

        if (left + 64 >= right) {
          for (int i = left; i <= right; i++) {
            if (arr[i].prc == target) return i;
          }
          return -1;
        }

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }
    return -1;
}

// not helpful
__always_inline int binary_search_reverse_liner(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left <= right) {
      int mid = left + ((right - left) >> 1);

        if (arr[mid].prc == target) return mid;

        if (left + 64 >= right) {
          for (int i = right; i >= left; i--) {
            if (arr[i].prc == target) return i;
          }
          return -1;
        }

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }
    return -1;
}

__always_inline int binary_search_pragma(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left <= right) {
      int mid = left + ((right - left) >> 1);

        if (arr[mid].prc == target) return mid;
        
        if (left + 64 >= right) {
          #pragma GCC ivdep // ivdep: IgnoreVectorDEPendencies, 告诉编译器忽略循环中的潜在数据依赖，允许进行向量化优化。
          for (int i = left; i <= right; i++) {
            if (arr[i].prc == target) return i;
          }
          return -1;
        }

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }
    return -1;
}

// not helpful
__always_inline int binary_search_unloop(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left <= right) {
      int mid = left + ((right - left) >> 1);

        if (arr[mid].prc == target) return mid;
        
        if (left + 64 >= right) {
          #pragma GCC unroll 8
          for (int i = left; i <= right; i++) {
            if (arr[i].prc == target) return i;
          }
          return -1;
        }

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }
    return -1;
}



__always_inline int binary_search_simd(std::vector<PV>& arr, int target) noexcept {
  int left = 0, right = arr.size() - 1;
    
  while (left <= right) {
      int mid = left + ((right - left) >> 1);
      if (arr[mid].prc == target) return mid;
      
      if (right - left <= 64) {
          __m256i target_vec = _mm256_set1_epi64x(target);
          int i = left;

          for (; i + 3 <= right; i += 4) {
              __m256i prc_vec = _mm256_set_epi64x(
                  arr[i + 3].prc,
                  arr[i + 2].prc,
                  arr[i + 1].prc,
                  arr[i + 0].prc
              );
              
              __m256i cmp = _mm256_cmpeq_epi64(prc_vec, target_vec);
              int mask = _mm256_movemask_pd(_mm256_castsi256_pd(cmp));
              
              if (mask) {
                  return i + __builtin_ctz(mask);
              }
          }
          
          // 处理剩余元素
          for (; i <= right; ++i) {
              if (arr[i].prc == target) return i;
          }
          return -1;
      }
      
      if (arr[mid].prc < target) 
          left = mid + 1;
      else 
          right = mid - 1;
  }
  return -1;
}

/**
 * 在 Haswell/Skylake/Cascade Lake 等多数 Intel 核上，gather 是微码实现，延迟高、吞吐低，远慢于4条独立的标量 mov 加载。
 * 即使在 Ice Lake 以后有改进，命中 L1/L2 时也常常不如 4 次标量 load。
 * 一条 gather 要串行完成 4 个地址生成与潜在的 4 次内存访问，难以与其他指令交叠；而 4 条独立的 mov 能被乱序核心并行发射，利用 ILP。
 */
__always_inline int binary_search_simd_gather(std::vector<PV>& arr, int64_t target) noexcept {
  int left = 0, right = arr.size() - 1;
  
  while (left <= right) {
      int mid = left + ((right - left) >> 1);
      
      if (arr[mid].prc == target) return mid;
      
      // 当范围小于等于32时，使用SIMD线性搜索
      if (right - left <= 64) {
          __m256i target_vec = _mm256_set1_epi64x(target);
          
          int i = left;
          // 使用gather指令收集非连续的prc值
          for (; i + 3 <= right; i += 4) {
              // 创建索引（每个PV结构体16字节，prc在偏移0处）
              __m256i indices = _mm256_set_epi64x(
                  (i + 3) * sizeof(PV),
                  (i + 2) * sizeof(PV),
                  (i + 1) * sizeof(PV),
                  (i + 0) * sizeof(PV)
              );
              
              // 使用gather从非连续地址加载4个int64
              __m256i prc_vec = _mm256_i64gather_epi64(
                  (const long long*)&arr[0], 
                  indices, 
                  1  // scale = 1
              );
              
              // 比较
              __m256i cmp = _mm256_cmpeq_epi64(prc_vec, target_vec);
              int mask = _mm256_movemask_pd(_mm256_castsi256_pd(cmp));
              
              if (mask) {
                  return i + __builtin_ctz(mask);
              }
          }
          
          // 处理剩余元素
          for (; i <= right; ++i) {
              if (arr[i].prc == target) return i;
          }
          return -1;
      }
      
      if (arr[mid].prc < target) 
          left = mid + 1;
      else 
          right = mid - 1;
  }
  return -1;
}