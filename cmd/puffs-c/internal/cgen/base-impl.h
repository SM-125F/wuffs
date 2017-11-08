// After editing this file, run "go generate" in this directory.

#ifndef PUFFS_BASE_IMPL_H
#define PUFFS_BASE_IMPL_H

// Copyright 2017 The Puffs Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// puffs_base__empty_struct is used when a Puffs function returns an empty
// struct. In C, if a function f returns void, you can't say "x = f()", but in
// Puffs, if a function g returns empty, you can say "y = g()".
typedef struct {
} puffs_base__empty_struct;

#define PUFFS_BASE__IGNORE_POTENTIALLY_UNUSED_VARIABLE(x) (void)(x)

// PUFFS_BASE__MAGIC is a magic number to check that initializers are called.
// It's not foolproof, given C doesn't automatically zero memory before use,
// but it should catch 99.99% of cases.
//
// Its (non-zero) value is arbitrary, based on md5sum("puffs").
#define PUFFS_BASE__MAGIC (0xCB3699CCU)

// PUFFS_BASE__ALREADY_ZEROED is passed from a container struct's initializer
// to a containee struct's initializer when the container has already zeroed
// the containee's memory.
//
// Its (non-zero) value is arbitrary, based on md5sum("zeroed").
#define PUFFS_BASE__ALREADY_ZEROED (0x68602EF1U)

// Use switch cases for coroutine suspension points, similar to the technique
// in https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
//
// We use trivial macros instead of an explicit assignment and case statement
// so that clang-format doesn't get confused by the unusual "case"s.
#define PUFFS_BASE__COROUTINE_SUSPENSION_POINT_0 case 0:;
#define PUFFS_BASE__COROUTINE_SUSPENSION_POINT(n) \
  coro_susp_point = n;                            \
  case n:;

#define PUFFS_BASE__COROUTINE_SUSPENSION_POINT_MAYBE_SUSPEND(n) \
  if (status < 0) {                                             \
    goto exit;                                                  \
  } else if (status == 0) {                                     \
    goto ok;                                                    \
  }                                                             \
  coro_susp_point = n;                                          \
  goto suspend;                                                 \
  case n:;

// Clang also defines "__GNUC__".
#if defined(__GNUC__)
#define PUFFS_BASE__LIKELY(expr) (__builtin_expect(!!(expr), 1))
#define PUFFS_BASE__UNLIKELY(expr) (__builtin_expect(!!(expr), 0))
#else
#define PUFFS_BASE__LIKELY(expr) (expr)
#define PUFFS_BASE__UNLIKELY(expr) (expr)
#endif

// Uncomment this #include for printf-debugging.
// #include <stdio.h>

// ---------------- Static Inline Functions
//
// The helpers below are functions, instead of macros, because their arguments
// can be an expression that we shouldn't evaluate more than once.
//
// They are in base-impl.h and hence copy/pasted into every generated C file,
// instead of being in some "base.c" file, since a design goal is that users of
// the generated C code can often just #include a single .c file, such as
// "gif.c", without having to additionally include or otherwise build and link
// a "base.c" file.
//
// They are static, so that linking multiple puffs .o files won't complain about
// duplicate function definitions.
//
// They are explicitly marked inline, even if modern compilers don't use the
// inline attribute to guide optimizations such as inlining, to avoid the
// -Wunused-function warning, and we like to compile with -Wall -Werror.

static inline uint16_t puffs_base__load_u16be(uint8_t* p) {
  return ((uint16_t)(p[0]) << 8) | ((uint16_t)(p[1]) << 0);
}

static inline uint16_t puffs_base__load_u16le(uint8_t* p) {
  return ((uint16_t)(p[0]) << 0) | ((uint16_t)(p[1]) << 8);
}

static inline uint32_t puffs_base__load_u32be(uint8_t* p) {
  return ((uint32_t)(p[0]) << 24) | ((uint32_t)(p[1]) << 16) |
         ((uint32_t)(p[2]) << 8) | ((uint32_t)(p[3]) << 0);
}

static inline uint32_t puffs_base__load_u32le(uint8_t* p) {
  return ((uint32_t)(p[0]) << 0) | ((uint32_t)(p[1]) << 8) |
         ((uint32_t)(p[2]) << 16) | ((uint32_t)(p[3]) << 24);
}

static inline puffs_base__slice_u8 puffs_base__slice_u8__subslice_i(
    puffs_base__slice_u8 s,
    uint64_t i) {
  if ((i <= SIZE_MAX) && (i <= s.len)) {
    return ((puffs_base__slice_u8){
        .ptr = s.ptr + i,
        .len = s.len - i,
    });
  }
  return ((puffs_base__slice_u8){});
}

static inline puffs_base__slice_u8 puffs_base__slice_u8__subslice_j(
    puffs_base__slice_u8 s,
    uint64_t j) {
  if ((j <= SIZE_MAX) && (j <= s.len)) {
    return ((puffs_base__slice_u8){.ptr = s.ptr, .len = j});
  }
  return ((puffs_base__slice_u8){});
}

static inline puffs_base__slice_u8 puffs_base__slice_u8__subslice_ij(
    puffs_base__slice_u8 s,
    uint64_t i,
    uint64_t j) {
  if ((i <= j) && (j <= SIZE_MAX) && (j <= s.len)) {
    return ((puffs_base__slice_u8){
        .ptr = s.ptr + i,
        .len = j - i,
    });
  }
  return ((puffs_base__slice_u8){});
}

// puffs_base__slice_u8__prefix returns up to the first up_to bytes of s.
static inline puffs_base__slice_u8 puffs_base__slice_u8__prefix(
    puffs_base__slice_u8 s,
    uint64_t up_to) {
  if ((uint64_t)(s.len) > up_to) {
    s.len = up_to;
  }
  return s;
}

// puffs_base__slice_u8__suffix returns up to the last up_to bytes of s.
static inline puffs_base__slice_u8 puffs_base__slice_u8_suffix(
    puffs_base__slice_u8 s,
    uint64_t up_to) {
  if ((uint64_t)(s.len) > up_to) {
    s.ptr += (uint64_t)(s.len) - up_to;
    s.len = up_to;
  }
  return s;
}

// puffs_base__slice_u8__copy_from_slice calls memmove(dst.ptr, src.ptr,
// length) where length is the minimum of dst.len and src.len.
//
// Passing a puffs_base__slice_u8 with all fields NULL or zero (a valid, empty
// slice) is valid and results in a no-op.
static inline uint64_t puffs_base__slice_u8__copy_from_slice(
    puffs_base__slice_u8 dst,
    puffs_base__slice_u8 src) {
  size_t length = dst.len < src.len ? dst.len : src.len;
  if (length > 0) {
    memmove(dst.ptr, src.ptr, length);
  }
  return length;
}

static inline uint32_t puffs_base__writer1__copy_from_history32(
    uint8_t** ptr_ptr,
    uint8_t* start,  // May be NULL, meaning an unmarked writer1.
    uint8_t* end,
    uint32_t distance,
    uint32_t length) {
  if (!start) {
    return 0;
  }
  uint8_t* ptr = *ptr_ptr;
  size_t d = ptr - start;
  if ((d == 0) || (d < (size_t)(distance))) {
    return 0;
  }
  start = ptr - distance;
  size_t n = end - ptr;
  if ((size_t)(length) > n) {
    length = n;
  } else {
    n = length;
  }
  // TODO: unrolling by 3 seems best for the std/flate benchmarks, but that is
  // mostly because 3 is the minimum length for the flate format. This function
  // implementation shouldn't overfit to that one format. Perhaps the
  // copy_from_history32 Puffs method should also take an unroll hint argument,
  // and the cgen can look if that argument is the constant expression '3'.
  //
  // Alternatively, or additionally, have a sloppy_copy_from_history32 method
  // that copies 8 bytes at a time, possibly writing more than length bytes?
  for (; n >= 3; n -= 3) {
    *ptr++ = *start++;
    *ptr++ = *start++;
    *ptr++ = *start++;
  }
  for (; n; n--) {
    *ptr++ = *start++;
  }
  *ptr_ptr = ptr;
  return length;
}

static inline uint32_t puffs_base__writer1__copy_from_reader32(
    uint8_t** ptr_wptr,
    uint8_t* wend,
    uint8_t** ptr_rptr,
    uint8_t* rend,
    uint32_t length) {
  uint8_t* wptr = *ptr_wptr;
  size_t n = length;
  if (n > wend - wptr) {
    n = wend - wptr;
  }
  uint8_t* rptr = *ptr_rptr;
  if (n > rend - rptr) {
    n = rend - rptr;
  }
  if (n > 0) {
    memmove(wptr, rptr, n);
    *ptr_wptr += n;
    *ptr_rptr += n;
  }
  return n;
}

static inline uint64_t puffs_base__writer1__copy_from_slice(
    uint8_t** ptr_wptr,
    uint8_t* wend,
    puffs_base__slice_u8 src) {
  uint8_t* wptr = *ptr_wptr;
  size_t n = src.len;
  if (n > wend - wptr) {
    n = wend - wptr;
  }
  if (n > 0) {
    memmove(wptr, src.ptr, n);
    *ptr_wptr += n;
  }
  return n;
}

static inline uint32_t puffs_base__writer1__copy_from_slice32(
    uint8_t** ptr_wptr,
    uint8_t* wend,
    puffs_base__slice_u8 src,
    uint32_t length) {
  uint8_t* wptr = *ptr_wptr;
  size_t n = src.len;
  if (n > length) {
    n = length;
  }
  if (n > wend - wptr) {
    n = wend - wptr;
  }
  if (n > 0) {
    memmove(wptr, src.ptr, n);
    *ptr_wptr += n;
  }
  return n;
}

// Note that the *__limit and *__mark methods are private (in base-impl.h) not
// public (in base-header.h). We assume that, at the boundary between user code
// and Puffs code, the reader1 and writer1's private_impl fields (including
// limit and mark) are NULL. Otherwise, some internal assumptions break down.
// For example, limits could be represented as pointers, even though
// conceptually they are counts, but that pointer-to-count correspondence
// becomes invalid if a buffer is re-used (e.g. on resuming a coroutine).
//
// Admittedly, some of the Puffs test code calls these methods, but that test
// code is still Puffs code, not user code. Other Puffs test code modifies
// private_impl fields directly.

static inline puffs_base__reader1 puffs_base__reader1__limit(
    puffs_base__reader1* o,
    uint64_t* ptr_to_len) {
  puffs_base__reader1 ret = *o;
  ret.private_impl.limit.ptr_to_len = ptr_to_len;
  ret.private_impl.limit.next = &o->private_impl.limit;
  return ret;
}

static inline puffs_base__empty_struct puffs_base__reader1__mark(
    puffs_base__reader1* o,
    uint8_t* mark) {
  o->private_impl.mark = mark;
  return ((puffs_base__empty_struct){});
}

// TODO: static inline puffs_base__writer1 puffs_base__writer1__limit()

static inline puffs_base__empty_struct puffs_base__writer1__mark(
    puffs_base__writer1* o,
    uint8_t* mark) {
  o->private_impl.mark = mark;
  return ((puffs_base__empty_struct){});
}

#endif  // PUFFS_BASE_IMPL_H
