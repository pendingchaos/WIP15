#include "../gui.h"
#include "../utils.h"

static void init(object_tab_t* tab) {
    add_obj_common_to_info_box(tab->info_box);
}

static void deinit(object_tab_t* tab) {}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    if (!set_obj_common_at_info_box(tab->info_box, rev_head, revision)) return;
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcTransformFeedback] = &init;
    object_tab_deinit_callbacks[(int)TrcTransformFeedback] = &deinit;
    object_tab_update_callbacks[(int)TrcTransformFeedback] = (object_tab_update_callback_t)&update;
}
