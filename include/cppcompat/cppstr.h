#ifndef CPPSTR_H
#define CPPSTR_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Pure C construction of MSVC-compatible std::string (32 bytes, x64).
 *
 * Memory layout:
 *   [0-15]  char _Buf[16] | char *_Ptr   (union)
 *   [16-23] size_t _Mysize
 *   [24-31] size_t _Myres  (15 for SSO)
 *
 * Heap strings use plain malloc (< 4096 bytes) to match MSVC's allocator.
 */

#define CPPSTR_SIZE  32
#define CPPSTR_MAX   4095

/* Heap-allocate a std::string. Caller must cppstr_free(). */
void *cppstr_new(const char *str);

/* Construct a std::string in-place at dst (e.g. packet field). Do NOT free. */
void cppstr_place(void *dst, const char *str);

/* Get C string pointer. Valid while the object lives. strdup() to keep. */
const char *cppstr_str(const void *s);

/*
 * Destroy a std::string.
 * Always frees the internal heap buffer.
 * If free_obj is true, also frees the 32-byte object itself.
 */
void cppstr_free(void *s, bool free_obj);

#ifdef __cplusplus
}
#endif

#endif
