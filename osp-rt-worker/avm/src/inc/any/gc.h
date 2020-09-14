/* Copyright (c) 2017 Nguyen Viet Giang. All rights reserved. */
#pragma once

#include <any/rt_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Initialize as a new garbage collector.
ANY_API aerror_t
agc_init(
    agc_t* self, aint_t heap_cap, aalloc_t alloc, void* alloc_ud);

/// Release all dynamic allocated object.
ANY_API void
agc_cleanup(
    agc_t* self);

/// Check if there are enough space for `n` new object.
static AINLINE int32_t
agc_check(
    agc_t* self, aint_t sz, aint_t n)
{
    aint_t more = sz + (n * sizeof(agc_header_t));
    aint_t new_heap_sz = self->heap_sz + more;
    return new_heap_sz <= self->heap_cap ? TRUE : FALSE;
}

/** Allocate a new collectable object.
\brief Returns the `heap_idx` of allocated object.
*/
ANY_API aint_t
agc_alloc(
    agc_t* self, atype_t type, aint_t sz);

/// Ensures that there are `more` bytes for `n` new objects in the heap.
ANY_API aerror_t
agc_reserve(
    agc_t* self, aint_t more, aint_t n);

/** Reclaim unreferenced objects.
\brief `root` must be NULL terminated.
*/
ANY_API void
agc_collect(
    agc_t* self, avalue_t** roots, aint_t* num_roots);

/// Get current heap size.
static AINLINE aint_t
agc_heap_size(
    agc_t* self)
{
    return self->heap_sz;
}

#ifdef __cplusplus
} // extern "C"
#endif