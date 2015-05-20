/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2014 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/base/memory.h"

#include <algorithm>

#if !XE_PLATFORM_WIN32
#include <unistd.h>
#endif  // !XE_PLATFORM_WIN32

namespace xe {

size_t page_size() {
  static size_t value = 0;
  if (!value) {
#if XE_PLATFORM_WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    value = si.dwAllocationGranularity;
#else
    value = getpagesize();
#endif  // XE_PLATFORM_WIN32
  }
  return value;
}

// TODO(benvanik): fancy AVX versions.
// http://gnuradio.org/redmine/projects/gnuradio/repository/revisions/cb32b70b79f430456208a2cd521d028e0ece5d5b/entry/volk/kernels/volk/volk_16u_byteswap.h
// http://gnuradio.org/redmine/projects/gnuradio/repository/revisions/f2bc76cc65ffba51a141950f98e75364e49df874/entry/volk/kernels/volk/volk_32u_byteswap.h
// http://gnuradio.org/redmine/projects/gnuradio/repository/revisions/2c4c371885c31222362f70a1cd714415d1398021/entry/volk/kernels/volk/volk_64u_byteswap.h

void copy_and_swap_16_aligned(uint16_t* dest, const uint16_t* src,
                              size_t count) {
  return copy_and_swap_16_unaligned(dest, src, count);
}

void copy_and_swap_16_unaligned(uint16_t* dest, const uint16_t* src,
                                size_t count) {
  for (size_t i = 0; i < count; ++i) {
    dest[i] = byte_swap(src[i]);
  }
}

void copy_and_swap_32_aligned(uint32_t* dest, const uint32_t* src,
                              size_t count) {
  return copy_and_swap_32_unaligned(dest, src, count);
}

void copy_and_swap_32_unaligned(uint32_t* dest, const uint32_t* src,
                                size_t count) {
  for (size_t i = 0; i < count; ++i) {
    dest[i] = byte_swap(src[i]);
  }
}

void copy_and_swap_64_aligned(uint64_t* dest, const uint64_t* src,
                              size_t count) {
  return copy_and_swap_64_unaligned(dest, src, count);
}

void copy_and_swap_64_unaligned(uint64_t* dest, const uint64_t* src,
                                size_t count) {
  for (size_t i = 0; i < count; ++i) {
    dest[i] = byte_swap(src[i]);
  }
}

}  // namespace xe
