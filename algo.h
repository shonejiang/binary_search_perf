#include "util.h"
#pragma once
#include <array>

#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif


__always_inline int binary_search_stl(std::vector<PV>& arr, int target) noexcept {
  auto it = std::lower_bound(arr.begin(), arr.end(), target, [](const PV& a, int b) { return a.prc < b; });
  if (it != arr.end() && it->prc == target) return it - arr.begin();
  return -1;
}

__always_inline int binary_search_branch(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size();
    while (left < right) {
      int mid = left + ((right - left) / 2);
      if (arr[mid].prc < target) 
          left = mid + 1;
      else 
          right = mid;
    }

    return (left < (int)arr.size() && arr[left].prc == target) ? left : -1;
}

__always_inline int binary_search_bit_op(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size();
    while (left < right) {
      int mid = left + ((right - left) >> 1);
      if (arr[mid].prc < target) 
          left = mid + 1;
      else 
          right = mid;
    }

    return (left < (int)arr.size() && arr[left].prc == target) ? left : -1;
}

__always_inline int binary_search_unlikely(std::vector<PV>& arr, int target) noexcept {
  int left = 0;
  int right = arr.size();
  
  while (left < right) {
    int mid = left + ((right - left) >> 1);
    
    if (arr[mid].prc <= target) [[unlikely]] {
      left = mid + 1;  // 目标在右半部分或就是 mid
    } else {
      right = mid;  // 目标在左半部分
    }
  }
  
  return (left < (int)arr.size() && arr[left].prc == target) ? left : -1;
}

/**
 * ternary operator is more likely to trigger the `cmov` instruction than `if else`？
 * the result truns out this assumption is wrong.
 */
__always_inline int binary_search_ternary(std::vector<PV>& arr, int target) noexcept {
  int left = 0, right = arr.size();
  while (left < right) {
    int mid = left + ((right - left) >> 1);    
    
    left = (arr[mid].prc < target) ? mid + 1 : left;
    right = (arr[mid].prc < target) ? right : mid;
  }
  return (left < (int)arr.size() && arr[left].prc == target) ? left : -1;
}

/**
 * in binary_search_ternary(), the cost of cmov instruction is only 1 cycle, the memory access is also low.
 * in array, it also aims for removing the branch, but construct the 2 item array is more than 1 cycle.
 * prefer the ternary OP over the array.
 */
__always_inline int binary_search_array(std::vector<PV>& arr, int target) noexcept {
  int left = 0, right = arr.size();
  while (left < right) {
    int mid = left + ((right - left) >> 1);    
    
    left = std::array<int, 2>{left, mid + 1}[arr[mid].prc < target];
    right = std::array<int, 2>{mid, right}[arr[mid].prc < target];

  }
  return (left < (int)arr.size() && arr[left].prc == target) ? left : -1;
}


__always_inline int binary_search_branchless_mask(std::vector<PV>& arr, int target) noexcept {
  int left = 0, right = arr.size();
  while (left < right) {
    int mid = left + ((right - left) >> 1);
    
    int mask = -(arr[mid].prc < target);
    
    left = (mask & (mid + 1)) | (~mask & left);
    right = (~mask & (mid)) | (mask & right);
  }

  return (left < arr.size() && arr[left].prc == target) ? left : -1;
}

__always_inline int binary_search_75percent(std::vector<PV>& arr, int target) noexcept {
  int left = 0, right = arr.size(); // [left, right)
  while (left < right) {
    int len = right - left;
    int off = (len + 3) >> 2; // ceil(len/4), ensure off>=1
    int mid = right - off;    // left <= mid < right
    if (arr[mid].prc < target) left = mid + 1; else right = mid; // shrink
  }
  return (left < (int)arr.size() && arr[left].prc == target) ? left : -1;
}


__always_inline int less_instruction_branchless_return_at_end(std::vector<PV>& arr, int target) noexcept {
  int size = arr.size();
  int left = 0;
  int len = size;
  
  while (len > 1) {
      int half = len >> 1;
      int mid = left + half;
      
      left = arr[mid].prc < target ? mid : left;
      len -= half;
  }
  
  return (left < size && arr[left].prc == target) ? left : -1;
}

__always_inline int less_instruction_branchless_return_midway(std::vector<PV>& arr, int target) noexcept {
  int size = arr.size();
  int left = 0;
  int len = size;
  
  while (len > 1) {
      int half = len >> 1;
      int mid = left + half;
      if (arr[mid].prc == target) return mid;

      left = arr[mid].prc < target ? mid : left;
      len -= half;
  }
  
  return -1;
}

__always_inline int less_instruction_branchless_likely(std::vector<PV>& arr, int target) noexcept {
  int size = arr.size();
  int left = 0;
  int len = size;

  while (len > 1) {
      int half = len >> 1;
      int mid = left + half;
      
      left = __builtin_expect(arr[mid].prc < target, 1) ? mid : left;
      len -= half;
  }
  
  return (left < size && arr[left].prc == target) ? left : -1;
}

__always_inline int less_instruction_branchless_75p_and_return_at_end(std::vector<PV>& arr, int target) noexcept {
  int size = arr.size();
  int left = 0;
  int len = size;
  
  while (len > 1) {
      int half = (len >> 1) + (len >> 2);
      int mid = left + half;
      
      left = arr[mid].prc < target ? mid : left;
      len -= half;
  }
  
  return (left < size && arr[left].prc == target) ? left : -1;
}

__always_inline int less_instruction_branchless_75p_and_return_at_mid(std::vector<PV>& arr, int target) noexcept {
  int size = arr.size();
  int left = 0;
  int len = size;
  
  while (len > 1) {
      int half = (len >> 1) + (len >> 2);
      int mid = left + half;

      if (arr[mid].prc == target) return mid;
      
      left = arr[mid].prc < target ? mid : left;
      len -= half;
  }
  
  return (left < size && arr[left].prc == target) ? left : -1;
}

__always_inline int binary_search_liner(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left + 32 <= right) {
      int mid = left + ((right - left) >> 1);
        if (arr[mid].prc == target) return mid;

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }

    for (int i = left; i <= right; i++) {
      if (arr[i].prc == target) return i;
    }
    return -1;
}

__always_inline int binary_search_liner_75(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left + 32 <= right) {
      int mid = left + ((right - left) >> 1) + ((right - left) >> 2);
        if (arr[mid].prc == target) return mid;

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }

    for (int i = left; i <= right; i++) {
      if (arr[i].prc == target) return i;
    }
    return -1;
}

// not helpful
__always_inline int binary_search_reverse_liner(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left + 32 <= right) {
      int mid = left + ((right - left) >> 1);
        if (arr[mid].prc == target) return mid;

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }

    for (int i = right; i >= left; i--) {
      if (arr[i].prc == target) return i;
    }
    return -1;
}

__always_inline int binary_search_pragma(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left + 32 <= right) {
      int mid = left + ((right - left) >> 1);
        if (arr[mid].prc == target) return mid;

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }

    #pragma GCC ivdep // ivdep: IgnoreVectorDEPendencies, 告诉编译器忽略循环中的潜在数据依赖，允许进行向量化优化。
    for (int i = left; i <= right; i++) {
      if (arr[i].prc == target) return i;
    }
    return -1;
}

// not helpful
__always_inline int binary_search_unloop(std::vector<PV>& arr, int target) noexcept {
    int left = 0, right = arr.size() - 1;
    while (left + 32 <= right) {
      int mid = left + ((right - left) >> 1);

        if (arr[mid].prc == target) return mid;

        if (arr[mid].prc < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }

    #pragma GCC unroll 8
    for (int i = left; i <= right; i++) {
      if (arr[i].prc == target) return i;
    }
    return -1;
}

__always_inline int binary_search_simd(std::vector<PV>& arr, int target) noexcept {
  int left = 0, right = arr.size() - 1;
    
  while (left + 32 <= right) {
      int mid = left + ((right - left) >> 1);
      if (arr[mid].prc == target) return mid;
      
      if (arr[mid].prc < target) 
          left = mid + 1;
      else 
          right = mid - 1;
  }
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

/**
 * 在 Haswell/Skylake/Cascade Lake 等多数 Intel 核上，gather 是微码实现，延迟高、吞吐低，远慢于4条独立的标量 mov 加载。
 * 即使在 Ice Lake 以后有改进，命中 L1/L2 时也常常不如 4 次标量 load。
 * 一条 gather 要串行完成 4 个地址生成与潜在的 4 次内存访问，难以与其他指令交叠；而 4 条独立的 mov 能被乱序核心并行发射，利用 ILP。
 */
__always_inline int binary_search_simd_gather(std::vector<PV>& arr, int64_t target) noexcept {
  int left = 0, right = arr.size() - 1;
  
  while (left + 32 <= right) {
      int mid = left + ((right - left) >> 1);
      
      if (arr[mid].prc == target) return mid;
      
      if (arr[mid].prc < target) 
          left = mid + 1;
      else 
          right = mid - 1;
  }

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



__always_inline int binary_23_reverse_unloop_ivdep(std::vector<PV>& arr, int target) noexcept {
  int size = arr.size();
  int left = 0;
  int len = size;
  
  while (len >= 32) {
      int half = (len >> 1) + (len >> 2);
      int mid = left + half;

      left = arr[mid].prc < target ? mid : left;
      len -= half;
  }
    
  #pragma GCC unroll 8
  #pragma GCC ivdep // ivdep: IgnoreVectorDEPendencies, 告诉编译器忽略循环中的潜在数据依赖，允许进行向量化优化。
  for (int i = left + len - 1; i >= left; --i) {
    if (arr[i].prc == target) return i;
  }
  return -1;
}

__always_inline int bit_75p_early_ret_linner_scan(std::vector<PV>& arr, int target) noexcept {
  int size = arr.size();
  int left = 0;
  int len = size;
  while (len > 4) {
    int half = (len >> 1) + (len >> 2);
    int mid = left + half;

    left = arr[mid].prc < target ? mid : left;
    len -= half;
  }

  for (int i = left; i < left + len; ++i) {
    if (arr[i].prc == target) return i;
  }
  return -1;
}
