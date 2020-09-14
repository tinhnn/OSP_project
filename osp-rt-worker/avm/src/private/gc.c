/* Copyright (c) 2017 Nguyen Viet Giang. All rights reserved. */
#include <any/gc.h>

#define GROW_FACTOR 2
#define NOT_FORWARED -1

static AINLINE void*
aalloc(
    agc_t* self, void* old, const aint_t sz)
{
    return self->alloc(self->alloc_ud, old, sz);
}

static AINLINE uint8_t*
low_heap(
    agc_t* self)
{
    return self->cur_heap < self->new_heap ? self->cur_heap : self->new_heap;
}

static AINLINE void
swap(
    agc_t* self)
{
    uint8_t* tmp = self->cur_heap;
    self->cur_heap = self->new_heap;
    self->new_heap = tmp;
}

static AINLINE void
copy(
    agc_t* self, avalue_t* v)
{
    agc_header_t* ogch;
    agc_header_t* ngch;
    if (v->tag.collectable == FALSE) return;
    ogch = (agc_header_t*)(self->cur_heap + v->v.heap_idx);
    ngch = (agc_header_t*)(self->new_heap + self->heap_sz);
    if (ogch->forwared == NOT_FORWARED) {
        memcpy(ngch, ogch, (size_t)ogch->sz);
        ogch->forwared = self->heap_sz;
        self->heap_sz += ogch->sz;
    }
    v->v.heap_idx = ogch->forwared;
}

static AINLINE void
copy_tuple(
    agc_t* self, agc_tuple_t* o)
{
    aint_t i;
    avalue_t* elements = (avalue_t*)(o + 1);
    for (i = 0; i < o->sz; ++i) {
        copy(self, elements + i);
    }
}

static AINLINE void
copy_array(
    agc_t* self, agc_array_t* o)
{
    aint_t i;
    avalue_t* elements = AGC_CAST(avalue_t, self, o->buff.v.heap_idx);
    for (i = 0; i < o->sz; ++i) {
        copy(self, elements + i);
    }
}

static AINLINE void
copy_table(
    agc_t* self, agc_table_t* o)
{
    aint_t i;
    avalue_t* elements = AGC_CAST(avalue_t, self, o->buff.v.heap_idx);
    for (i = 0; i < o->sz; ++i) {
        copy(self, elements + i * 2);
        copy(self, elements + i * 2 + 1);
    }
}

static AINLINE void
scan(
    agc_t* self, agc_header_t* gch)
{
    switch (gch->type) {
    case AVT_NIL:
    case AVT_PID:
    case AVT_BOOLEAN:
    case AVT_INTEGER:
    case AVT_REAL:
    case AVT_NATIVE_FUNC:
    case AVT_BYTE_CODE_FUNC:
    case AVT_FIXED_BUFFER:
        // nop
        break;
    case AVT_BUFFER:
        copy(self, &((agc_buffer_t*)(gch + 1))->buff);
        break;
    case AVT_STRING:
        // nop
        break;
    case AVT_TUPLE: {
        agc_tuple_t* o = (agc_tuple_t*)(gch + 1);
        copy_tuple(self, o);
        break;
    }
    case AVT_ARRAY: {
        agc_array_t* o = (agc_array_t*)(gch + 1);
        copy_array(self, o);
        copy(self, &o->buff);
        break;
    }
    case AVT_TABLE: {
        agc_table_t* o = (agc_table_t*)(gch + 1);
        copy_table(self, o);
        copy(self, &o->buff);
        break;
    }
    default: assert(!"bad value type");
    }
}

aerror_t
agc_init(
    agc_t* self, aint_t heap_cap, aalloc_t alloc, void* alloc_ud)
{
    self->alloc = alloc;
    self->alloc_ud = alloc_ud;
    self->cur_heap = (uint8_t*)aalloc(self, NULL, heap_cap*2);
    if (!self->cur_heap) return AERR_FULL;
    self->new_heap = self->cur_heap + heap_cap;
    self->heap_cap = heap_cap;
    self->heap_sz = 0;
    return AERR_NONE;
}

void
agc_cleanup(
    agc_t* self)
{
    aalloc(self, low_heap(self), 0);
    self->new_heap = NULL;
    self->cur_heap = NULL;
    self->heap_cap = 0;
    self->heap_sz = 0;
}

aint_t
agc_alloc(
    agc_t* self, atype_t type, aint_t sz)
{
    agc_header_t* gch;
    aint_t more = AALIGN_FORWARD(sz + sizeof(agc_header_t), 8);
    aint_t new_heap_sz = self->heap_sz + more;
    aint_t heap_idx = self->heap_sz;
    assert(new_heap_sz <= self->heap_cap);
    self->heap_sz = new_heap_sz;
    gch = ((agc_header_t*)(self->cur_heap + heap_idx));
    gch->type = type;
    gch->forwared = NOT_FORWARED;
    gch->sz = more;
    return heap_idx;
}

aerror_t
agc_reserve(
    agc_t* self, aint_t more, aint_t n)
{
    uint8_t* nh;
    aint_t new_cap = self->heap_cap;
    more += (n * sizeof(agc_header_t));
    while (new_cap < self->heap_sz + more) new_cap *= GROW_FACTOR;
    nh = (uint8_t*)aalloc(self, NULL, new_cap * 2);
    if (!nh) return AERR_FULL;
    memcpy(nh, self->cur_heap, (size_t)self->heap_sz);
    aalloc(self, low_heap(self), 0);
    self->cur_heap = nh;
    self->new_heap = nh + new_cap;
    self->heap_cap = new_cap;
    return AERR_NONE;
}

void
agc_collect(
    agc_t* self, avalue_t** roots, aint_t* num_roots)
{
    aint_t i;
    self->heap_sz = 0;
    self->scan = 0;
    for (; *roots; ++roots, ++num_roots) {
        for (i = 0; i < *num_roots; ++i) {
            copy(self, *roots + i);
        }
    }
    while (self->scan != self->heap_sz) {
        agc_header_t* header = (agc_header_t*)(self->new_heap + self->scan);
        scan(self, header);
        self->scan += header->sz;
    }
    swap(self);
}
