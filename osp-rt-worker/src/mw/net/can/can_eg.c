#include <any/std_io.h>

#include <any/actor.h>
#include <any/loader.h>
#include <any/std_string.h>
#include <any/rt_types.h>
#include <stdio.h>

#include <lwlog/lwlog.h>
#include "can_eg.h"
static void lsend(aactor_t* a) {
    aint_t a_str = any_check_index(a, -1);
    int i=1;
    for (;;) {
        // printf("%s %d\n", any_to_string(a, a_str), i++);
        lwlog_info("%s %d\n", any_to_string(a, a_str), i++);
        any_yield(a);
    }
    any_push_nil(a);
    return;
}
static alib_func_t funcs[] = {
    { "send/1", &lsend },
    { NULL, NULL }
};

static alib_t mod = { "can-eg", funcs };

void anet_lib_add_can_eg(aloader_t* l, void* ud) {
    aloader_add_lib(l, &mod);
    return;
}