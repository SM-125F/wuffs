#ifndef PUFFS_GIF_H
#define PUFFS_GIF_H

// Code generated by puffs-gen-c. DO NOT EDIT.

#ifndef PUFFS_BASE_HEADER_H
#define PUFFS_BASE_HEADER_H

// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Puffs requires a word size of at least 32 bits because it assumes that
// converting a u32 to usize will never overflow. For example, the size of a
// decoded image is often represented, explicitly or implicitly in an image
// file, as a u32, and it is convenient to compare that to a buffer size.
#if __WORDSIZE < 32
#error "Puffs requires a word size of at least 32 bits"
#endif

// PUFFS_VERSION is the major.minor version number as a uint32. The major
// number is the high 16 bits. The minor number is the low 16 bits.
//
// The intention is to bump the version number at least on every API / ABI
// backwards incompatible change.
//
// For now, the API and ABI are simply unstable and can change at any time.
//
// TODO: don't hard code this in base-header.h.
#define PUFFS_VERSION (0x00001)

// puffs_base_buf1 is a 1-dimensional buffer (a pointer and length) plus
// additional indexes into that buffer.
//
// A value with all fields NULL or zero is a valid, empty buffer.
typedef struct {
  uint8_t* ptr;  // Pointer.
  size_t len;    // Length.
  size_t wi;     // Write index. Invariant: wi <= len.
  size_t ri;     // Read  index. Invariant: ri <= wi.
} puffs_base_buf1;

#endif  // PUFFS_BASE_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif

// ---------------- Status Codes

// Status codes are non-positive integers.
//
// The least significant bit indicates a non-recoverable status code: an error.
typedef enum {
  puffs_gif_status_ok = 0,
  puffs_gif_error_bad_version = -2 + 1,
  puffs_gif_error_bad_receiver = -4 + 1,
  puffs_gif_error_bad_argument = -6 + 1,
  puffs_gif_error_constructor_not_called = -8 + 1,
  puffs_gif_error_unexpected_eof = -10 + 1,
  puffs_gif_status_short_read = -12,
  puffs_gif_status_short_write = -14,
  puffs_gif_error_bad_gif_image = -256 + 1,
} puffs_gif_status;

bool puffs_gif_status_is_error(puffs_gif_status s);

const char* puffs_gif_status_string(puffs_gif_status s);

// ---------------- Public Structs

typedef struct {
  puffs_gif_status status;
  uint32_t magic;
  uint32_t f_literal_width;
  uint8_t f_stack[4096];
  uint8_t f_suffixes[4096];
  uint16_t f_prefixes[4096];
} puffs_gif_lzw_decoder;

// ---------------- Public Constructor and Destructor Prototypes

// puffs_gif_lzw_decoder_constructor is a constructor function.
//
// It should be called before any other puffs_gif_lzw_decoder_* function.
//
// Pass PUFFS_VERSION and 0 for puffs_version and for_internal_use_only.
void puffs_gif_lzw_decoder_constructor(puffs_gif_lzw_decoder* self,
                                       uint32_t puffs_version,
                                       uint32_t for_internal_use_only);

void puffs_gif_lzw_decoder_destructor(puffs_gif_lzw_decoder* self);

// ---------------- Public Function Prototypes

puffs_gif_status puffs_gif_lzw_decoder_decode(puffs_gif_lzw_decoder* self,
                                              puffs_base_buf1* a_dst,
                                              puffs_base_buf1* a_src,
                                              bool a_src_final);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // PUFFS_GIF_H

// C HEADER ENDS HERE.

#ifndef PUFFS_BASE_IMPL_H
#define PUFFS_BASE_IMPL_H

#define PUFFS_LOW_BITS(x, n) ((x) & ((1 << (n)) - 1))

#endif  // PUFFS_BASE_IMPL_H

// ---------------- Status Codes Implementations

bool puffs_gif_status_is_error(puffs_gif_status s) {
  return s & 1;
}

const char* puffs_gif_status_strings[9] = {
    "gif: ok",
    "gif: bad version",
    "gif: bad receiver",
    "gif: bad argument",
    "gif: constructor not called",
    "gif: unexpected EOF",
    "gif: short read",
    "gif: short write",
    "gif: bad GIF image",
};

const char* puffs_gif_status_string(puffs_gif_status s) {
  s = -(s >> 1);
  if (0 <= s) {
    if (s < 8) {
      return puffs_gif_status_strings[s];
    }
    s -= 120;
    if ((8 <= s) && (s < 9)) {
      return puffs_gif_status_strings[s];
    }
  }
  return "gif: unknown status";
}

// ---------------- Private Structs

// ---------------- Private Constructor and Destructor Prototypes

// ---------------- Private Function Prototypes

// ---------------- Constructor and Destructor Implementations

// PUFFS_MAGIC is a magic number to check that constructors are called. It's
// not foolproof, given C doesn't automatically zero memory before use, but it
// should catch 99.99% of cases.
//
// Its (non-zero) value is arbitrary, based on md5sum("puffs").
#define PUFFS_MAGIC (0xCB3699CCU)

// PUFFS_ALREADY_ZEROED is passed from a container struct's constructor to a
// containee struct's constructor when the container has already zeroed the
// containee's memory.
//
// Its (non-zero) value is arbitrary, based on md5sum("zeroed").
#define PUFFS_ALREADY_ZEROED (0x68602EF1U)

void puffs_gif_lzw_decoder_constructor(puffs_gif_lzw_decoder* self,
                                       uint32_t puffs_version,
                                       uint32_t for_internal_use_only) {
  if (!self) {
    return;
  }
  if (puffs_version != PUFFS_VERSION) {
    self->status = puffs_gif_error_bad_version;
    return;
  }
  if (for_internal_use_only != PUFFS_ALREADY_ZEROED) {
    memset(self, 0, sizeof(*self));
  }
  self->magic = PUFFS_MAGIC;
  self->f_literal_width = 8;
}

void puffs_gif_lzw_decoder_destructor(puffs_gif_lzw_decoder* self) {
  if (!self) {
    return;
  }
}

// ---------------- Function Implementations

puffs_gif_status puffs_gif_lzw_decoder_decode(puffs_gif_lzw_decoder* self,
                                              puffs_base_buf1* a_dst,
                                              puffs_base_buf1* a_src,
                                              bool a_src_final) {
  if (!self) {
    return puffs_gif_error_bad_receiver;
  }
  puffs_gif_status status = self->status;
  if (status & 1) {
    return status;
  }
  if (self->magic != PUFFS_MAGIC) {
    status = puffs_gif_error_constructor_not_called;
    goto cleanup0;
  }
  if (!a_dst || !a_src) {
    status = puffs_gif_error_bad_argument;
    goto cleanup0;
  }

  uint32_t v_clear_code;
  uint32_t v_end_code;
  bool v_use_save_code;
  uint32_t v_save_code;
  uint32_t v_prev_code;
  uint32_t v_width;
  uint32_t v_bits;
  uint32_t v_n_bits;
  uint32_t v_code;
  uint32_t v_s;

  v_clear_code = (((uint32_t)(1)) << self->f_literal_width);
  v_end_code = (v_clear_code + 1);
  v_use_save_code = 0;
  v_save_code = v_end_code;
  v_prev_code = 0;
  v_width = (self->f_literal_width + 1);
  v_bits = 0;
  v_n_bits = 0;
label_0_continue:;
  while (true) {
    while (v_n_bits < v_width) {
      if (a_src->ri >= a_src->wi) {
        status = puffs_gif_status_short_read;
        goto cleanup0;
      }
      uint8_t t_0 = a_src->ptr[a_src->ri++];
      v_bits |= (((uint32_t)(t_0)) << v_n_bits);
      v_n_bits += 8;
    }
    v_code = PUFFS_LOW_BITS(v_bits, v_width);
    v_bits >>= v_width;
    v_n_bits -= v_width;
    if (v_code < v_clear_code) {
      if (a_dst->wi >= a_dst->len) {
        status = puffs_gif_status_short_write;
        goto cleanup0;
      }
      a_dst->ptr[a_dst->wi++] = ((uint8_t)(v_code));
      if (v_use_save_code) {
        self->f_suffixes[v_save_code] = ((uint8_t)(v_code));
        self->f_prefixes[v_save_code] = ((uint16_t)(v_prev_code));
      }
    } else if (v_code == v_clear_code) {
      v_use_save_code = false;
      v_save_code = v_end_code;
      v_prev_code = 0;
      v_width = (self->f_literal_width + 1);
      goto label_0_continue;
    } else if (v_code == v_end_code) {
      status = puffs_gif_status_ok;
      goto cleanup0;
    } else if (v_code <= v_save_code) {
      v_s = 4095;
      while (v_s != 0) {
        if (v_s == 0) {
          status = puffs_gif_error_bad_gif_image;
          goto cleanup0;
        }
        v_s -= 1;
      }
    } else {
      status = puffs_gif_error_bad_gif_image;
      goto cleanup0;
    }
    v_use_save_code = (v_save_code < 4095);
    if (v_use_save_code) {
      v_save_code += 1;
      if ((v_save_code == (((uint32_t)(1)) << v_width)) && (v_width < 12)) {
        v_width += 1;
      }
    }
    v_prev_code = v_code;
  }

cleanup0:
  self->status = status;
  return status;
}
