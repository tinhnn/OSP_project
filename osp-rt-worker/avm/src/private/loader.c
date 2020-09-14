/* Copyright (c) 2017 Nguyen Viet Giang. All rights reserved. */
#include <any/loader.h>

#include <any/version.h>
#include <any/list.h>

const achunk_header_t CHUNK_HEADER = {
    { 0x41, 0x6E, 0x79, 0x00 },
    AVERSION_MAJOR,
    AVERSION_MINOR,
    ABIG_ENDIAN,
    sizeof(aint_t),
    sizeof(areal_t),
    sizeof(ainstruction_t),
    { 0, 0 }
};

static aint_t
calc_sizes(
    int8_t* b, aint_t sz, aint_t* off, aint_t* num_imps, aint_t* num_protos)
{
    aint_t i;
    aprototype_header_t* const p = (aprototype_header_t*)(b + *off);

    *num_imps += p->num_imports; *num_protos += p->num_nesteds;
    *off += sizeof(aprototype_header_t) + p->strings_sz +
        sizeof(ainstruction_t) * p->num_instructions +
        sizeof(aconstant_t) * p->num_constants +
        sizeof(aimport_t) * p->num_imports +
        sizeof(aint_t) * p->num_instructions;
    if (*off > sz) return AERR_MALFORMED;

    for (i = 0; i < p->num_nesteds; ++i) {
        aerror_t ec = calc_sizes(b, sz, off, num_imps, num_protos);
        if (ec != AERR_NONE) return ec;
    }

    return AERR_NONE;
}

static void
free_chunk_list(
    aloader_t* self, alist_t* l, int32_t check_for_retain)
{
    alist_node_t* i = alist_head(l);
    while (!alist_is_end(l, i)) {
        achunk_t* c = ALIST_NODE_CAST(achunk_t, i);
        i = i->next;
        if (check_for_retain && c->retain) continue;
        alist_node_erase(&c->node);
        if (c->alloc) c->alloc(c->alloc_ud, c->header, 0);
        self->alloc(self->alloc_ud, c, 0);
    }
}

static void
free_libs(
    alist_t* l)
{
    alist_node_t* i = alist_head(l);
    while (!alist_is_end(l, i)) {
        alib_t* lib = ALIST_NODE_CAST(alib_t, i);
        i = i->next;
        alist_node_erase(&lib->node);
    }
}

static void
create_proto(
    achunk_t* chunk, aint_t* off,
    aprototype_t* pt, avalue_t** next_imp, aprototype_t** next_pt)
{
    aint_t i;
    int8_t* const b = (int8_t*)chunk->header;
    aprototype_header_t* const p = (aprototype_header_t*)(b + *off);

    pt->chunk = chunk;
    pt->header = p;
    pt->strings = (const char*)(p + 1);
    pt->instructions = (ainstruction_t*)(((uint8_t*)(p + 1)) + p->strings_sz);
    pt->constants = (aconstant_t*)(pt->instructions + p->num_instructions);
    pt->imports = (aimport_t*)(pt->constants + p->num_constants);
    pt->import_values = *next_imp; *next_imp += p->num_imports;
    pt->source_lines = (aint_t*)(pt->imports + p->num_imports);
    pt->nesteds = *next_pt; *next_pt += p->num_nesteds;
    *off += (uint8_t*)(pt->source_lines + p->num_instructions) - (uint8_t*)p;

    for (i = 0; i < p->num_nesteds; ++i) {
        create_proto(chunk, off, pt->nesteds + i, next_imp, next_pt);
    }
}

static aint_t
find_in_libs(
    alist_t* libs, const char* module, const char* name, avalue_t* value)
{
    alist_node_t* n;
    const alib_func_t* nf;

    for (n = alist_head(libs); !alist_is_end(libs, n); n = n->next) {
        alib_t* const lib = ALIST_NODE_CAST(alib_t, n);
        if (strcmp(lib->name, module) != 0) continue;
        // for each native module function
        for (nf = lib->funcs; nf->name != NULL; ++nf) {
            if (strcmp(nf->name, name) == 0) {
                av_native_func(value, nf->func);
                return AERR_NONE;
            }
        }
    }

    return AERR_UNRESOLVED;
}

static aint_t
find_in_list(
    alist_t* list, const char* module, const char* name, avalue_t* value)
{
    aint_t i;
    alist_node_t* n;

    for (n = alist_head(list); !alist_is_end(list, n); n = n->next) {
        achunk_t* const chunk = ALIST_NODE_CAST(achunk_t, n);
        aprototype_t* const m = chunk->prototypes;
        if (strcmp(m->strings + m->header->symbol, module) != 0) continue;
        // for each module function
        for (i = 0; i < m->header->num_nesteds; ++i) {
            aprototype_t* const f = m->nesteds + i;
            if (strcmp(f->strings + f->header->symbol, name) == 0) {
                av_byte_code_func(value, f);
                return AERR_NONE;
            }
        }
    }

    return AERR_UNRESOLVED;
}

static int32_t
has_chunk(
    alist_t* list, alist_node_t* node)
{
    aprototype_t* a = ALIST_NODE_CAST(achunk_t, node)->prototypes;
    const char* a_sym = a->strings + a->header->symbol;
    alist_node_t* i = alist_head(list);
    while (!alist_is_end(list, i)) {
        aprototype_t* b = ALIST_NODE_CAST(achunk_t, i)->prototypes;
        const char* b_sym = b->strings + b->header->symbol;
        if (strcmp(a_sym, b_sym) == 0) return TRUE;
        i = i->next;
    }
    return FALSE;
}

static aint_t
resolve(
    aloader_t* self, aprototype_t* p, int32_t unresolved_as_nil)
{
    aint_t i;
    aerror_t ec;

    // for each import
    for (i = 0; i < p->header->num_imports; ++i) {
        aimport_t* imp = p->imports + i;
        const char* m_name = p->strings + imp->module;
        const char* n_name = p->strings + imp->name;
        avalue_t* val = p->import_values + i;
        ec = find_in_libs(&self->libs, m_name, n_name, val);
        if (ec == AERR_NONE) continue;
        ec = find_in_list(&self->pendings, m_name, n_name, val);
        if (ec == AERR_NONE) continue;
        ec = find_in_list(&self->runnings, m_name, n_name, val);
        if (ec == AERR_NONE) continue;
        if (unresolved_as_nil) {
            av_nil(val);
            continue;
        }
        if (self->on_unresolved) {
            self->on_unresolved(self, m_name, n_name, self->on_unresolved_ud);
        }
        return AERR_UNRESOLVED;
    }

    // recursive for children
    for (i = 0; i < p->header->num_nesteds; ++i) {
        ec = resolve(self, p->nesteds + i, unresolved_as_nil);
        if (ec != AERR_NONE) return ec;
    }

    return AERR_NONE;
}

static aint_t
cacl_chunk_imports(
    aprototype_t* pt)
{
    aint_t i;
    aint_t acc = pt->header->num_imports;
    for (i = 0; i < pt->header->num_nesteds; ++i) {
        acc += cacl_chunk_imports(pt->nesteds + i);
    }
    return acc;
}

static aint_t
calc_imports(
    alist_t* list)
{
    aint_t acc = 0;
    alist_node_t* i = alist_head(list);
    while (!alist_is_end(list, i)) {
        achunk_t* chunk = ALIST_NODE_CAST(achunk_t, i);
        acc += cacl_chunk_imports(chunk->prototypes);
        i = i->next;
    }
    return acc;
}

static void
backup_imports(
    aprototype_t* pt, aint_t* off, avalue_t* imp)
{
    aint_t i;
    memcpy(
        imp + *off,
        pt->import_values,
        sizeof(avalue_t) * (size_t)pt->header->num_imports);
    *off += pt->header->num_imports;
    for (i = 0; i < pt->header->num_nesteds; ++i) {
        backup_imports(pt->nesteds + i, off, imp);
    }
}

static void
rollback_imports(
    aprototype_t* pt, aint_t* off, avalue_t* imp)
{
    aint_t i;
    memcpy(
        pt->import_values,
        imp + *off,
        sizeof(avalue_t) * (size_t)pt->header->num_imports);
    *off += pt->header->num_imports;
    for (i = 0; i < pt->header->num_nesteds; ++i) {
        rollback_imports(pt->nesteds + i, off, imp);
    }
}

void
aloader_init(
    aloader_t* self, aalloc_t alloc, void* alloc_ud)
{
    memset(self, 0, sizeof(aloader_t));
    self->alloc = alloc;
    self->alloc_ud = alloc_ud;
    alist_init(&self->pendings);
    alist_init(&self->runnings);
    alist_init(&self->garbages);
    alist_init(&self->libs);
}

void
aloader_cleanup(
    aloader_t* self)
{
    free_chunk_list(self, &self->pendings, FALSE);
    free_chunk_list(self, &self->runnings, FALSE);
    free_chunk_list(self, &self->garbages, FALSE);
    free_libs(&self->libs);
}

static void
create_module(
    achunk_t* chunk)
{
    aint_t off = sizeof(achunk_header_t);
    avalue_t* next_imp = chunk->imports;
    aprototype_t* next_pt = chunk->prototypes;
    aprototype_t* pt = next_pt++;
    create_proto(chunk, &off, pt, &next_imp, &next_pt);
}

aerror_t
aloader_add_chunk(
    aloader_t* self, achunk_header_t* chunk, aint_t chunk_sz,
    aalloc_t chunk_alloc, void* chunk_alloc_ud)
{
    achunk_t* c;
    aint_t off, num_imps, num_protos;
    aerror_t ec;

    if (chunk_sz < sizeof(achunk_header_t) ||
        memcmp(chunk, &CHUNK_HEADER, sizeof(achunk_header_t)) != 0)
        return AERR_MALFORMED;
    off = sizeof(achunk_header_t);

    num_imps = 0; num_protos = 1 /* include module proto */;
    ec = calc_sizes((int8_t*)chunk, chunk_sz, &off, &num_imps, &num_protos);
    if (ec != AERR_NONE) return ec;

    c = (achunk_t*)self->alloc(self->alloc_ud, NULL,
        sizeof(achunk_t) +
        num_imps * sizeof(avalue_t) +
        num_protos * sizeof(aprototype_t));
    c->header = chunk;
    c->chunk_sz = chunk_sz;
    c->alloc = chunk_alloc;
    c->alloc_ud = chunk_alloc_ud;
    c->imports = (avalue_t*)(((uint8_t*)c) + sizeof(achunk_t));
    c->prototypes = (aprototype_t*)(
        ((uint8_t*)c->imports) + num_imps * sizeof(avalue_t));
    create_module(c);
    c->retain = FALSE;
    alist_push_back(&self->pendings, &c->node);

    return AERR_NONE;
}

void
aloader_add_lib(
    aloader_t* self, alib_t* lib)
{
    alist_push_back(&self->libs, &lib->node);
}

aerror_t
aloader_link(
    aloader_t* self, int32_t safe)
{
    alist_node_t* const garbage_back = alist_back(&self->garbages);
    avalue_t* old_imps;
    alist_node_t* i;

    // copy old chunk to garbages
    i = alist_head(&self->runnings);
    while (!alist_is_end(&self->runnings, i)) {
        alist_node_t* const next = i->next;
        if (has_chunk(&self->pendings, i)) {
            alist_node_erase(i);
            alist_push_back(&self->garbages, i);
        }
        i = next;
    }

    // resolve pending imports
    i = alist_head(&self->pendings);
    while (!alist_is_end(&self->pendings, i)) {
        achunk_t* chunk = ALIST_NODE_CAST(achunk_t, i);
        aerror_t ec = resolve(self, chunk->prototypes, FALSE);
        if (ec != AERR_NONE) {
            if (!safe) return ec;
            // rollback garbages
            i = garbage_back;
            while (!alist_is_end(&self->garbages, i)) {
                alist_node_t* const next = i->next;
                alist_node_erase(i);
                alist_push_back(&self->runnings, i);
                i = next;
            }
            // empty pendings
            free_chunk_list(self, &self->pendings, FALSE);
            return ec;
        }
        i = i->next;
    }

    // resolve running imports
    if (safe) {
        aint_t imp_off = 0;
        aint_t imp_sz = calc_imports(&self->runnings) * sizeof(avalue_t);
        old_imps = (avalue_t*)self->alloc(self->alloc_ud, NULL, imp_sz);
        i = alist_head(&self->runnings);
        while (!alist_is_end(&self->runnings, i)) {
            achunk_t* chunk = ALIST_NODE_CAST(achunk_t, i);
            backup_imports(chunk->prototypes, &imp_off, old_imps);
            i = i->next;
        }
    }
    i = alist_head(&self->runnings);
    while (!alist_is_end(&self->runnings, i)) {
        achunk_t* chunk = ALIST_NODE_CAST(achunk_t, i);
        aerror_t ec = resolve(self, chunk->prototypes, FALSE);
        if (ec != AERR_NONE) {
            if (!safe) return ec;
            // rollback running imports
            i = alist_head(&self->runnings);
            while (!alist_is_end(&self->runnings, i)) {
                aint_t imp_off = 0;
                achunk_t* chunk = ALIST_NODE_CAST(achunk_t, i);
                rollback_imports(chunk->prototypes, &imp_off, old_imps);
                i = i->next;
            }
            // rollback garbages
            i = garbage_back;
            while (!alist_is_end(&self->garbages, i)) {
                alist_node_t* const next = i->next;
                alist_node_erase(i);
                alist_push_back(&self->runnings, i);
                i = next;
            }
            // empty pendings
            free_chunk_list(self, &self->pendings, FALSE);
            return ec;
        }
        i = i->next;
    }
    if (safe) self->alloc(self->alloc_ud, old_imps, 0);

    // resolve garbage imports
    i = alist_head(&self->garbages);
    while (!alist_is_end(&self->garbages, i)) {
        achunk_t* chunk = ALIST_NODE_CAST(achunk_t, i);
        resolve(self, chunk->prototypes, TRUE);
        i = i->next;
    }

    // move pendings to runnings
    i = alist_head(&self->pendings);
    while (!alist_is_end(&self->pendings, i)) {
        alist_node_t* const next = i->next;
        alist_node_erase(i);
        alist_push_back(&self->runnings, i);
        i = next;
    }

    if (self->on_linked) {
        self->on_linked(self, self->on_linked_ud);
    }

    return AERR_NONE;
}

void
aloader_sweep(
    aloader_t* self)
{
    free_chunk_list(self, &self->garbages, TRUE);
}

aerror_t
aloader_find(
    aloader_t* self, const char* module, const char* name, avalue_t* value)
{
    aerror_t ec;

    ec = find_in_libs(&self->libs, module, name, value);
    if (ec == AERR_NONE) return AERR_NONE;
    ec = find_in_list(&self->runnings, module, name, value);
    if (ec == AERR_NONE) return AERR_NONE;

    return AERR_UNRESOLVED;
}
