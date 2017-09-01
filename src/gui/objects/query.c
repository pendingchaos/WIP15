#include "../gui.h"
#include "../utils.h"

static void init(object_tab_t* tab) {
    add_obj_common_to_info_box(tab->info_box);
    add_to_info_box(tab->info_box, "Type");
    add_to_info_box(tab->info_box, "Result");
    add_to_info_box(tab->info_box, "Active Index");
}

static void deinit(object_tab_t* tab) {
    free_info_box(tab->data);
}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_query_rev_t* rev = (const trc_gl_query_rev_t*)rev_head;
    
    if (!set_obj_common_at_info_box(tab->info_box, rev_head, revision)) return;
    set_enum_at_info_box(tab->info_box, "Type", "QueryType", rev->type);
    set_at_info_box(tab->info_box, "Result", "%ld", rev->result);
    set_at_info_box(tab->info_box, "Active Index", "%d", rev->active_index);
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcQuery] = &init;
    object_tab_deinit_callbacks[(int)TrcQuery] = &deinit;
    object_tab_update_callbacks[(int)TrcQuery] = (object_tab_update_callback_t)&update;
}
