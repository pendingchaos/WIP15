#include "../gui.h"
#include "../utils.h"

static void init(object_tab_t* tab) {
    add_obj_common_to_info_box(tab->info_box);
    add_to_info_box(tab->info_box, "Type");
    add_to_info_box(tab->info_box, "Condition");
    add_to_info_box(tab->info_box, "Flags");
}

static void deinit(object_tab_t* tab) {}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_sync_rev_t* rev = (const trc_gl_sync_rev_t*)rev_head;
    
    if (!set_obj_common_at_info_box(tab->info_box, rev_head, revision)) return;
    set_enum_at_info_box(tab->info_box, "Type", "FenceType", rev->type);
    set_enum_at_info_box(tab->info_box, "Condition", "FenceCondition", rev->condition);
    set_at_info_box(tab->info_box, "Flags", ""); //There are no flags
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcSync] = &init;
    object_tab_deinit_callbacks[(int)TrcSync] = &deinit;
    object_tab_update_callbacks[(int)TrcSync] = (object_tab_update_callback_t)&update;
}
