#include "../gui.h"
#include "../utils.h"

static void init(object_tab_t* tab) {
    add_obj_common_to_info_box(tab->info_box);
    add_to_info_box(tab->info_box, "Active Program");
    add_to_info_box(tab->info_box, "Vertex Program");
    add_to_info_box(tab->info_box, "Fragment Program");
    add_to_info_box(tab->info_box, "Geometry Program");
    add_to_info_box(tab->info_box, "Tesselation Control Program");
    add_to_info_box(tab->info_box, "Tesselation Eval Program");
    add_to_info_box(tab->info_box, "Compute Program");
}

static void deinit(object_tab_t* tab) {}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_program_pipeline_rev_t* rev = (const trc_gl_program_pipeline_rev_t*)rev_head;
    
    info_box_t* box = tab->info_box;
    if (!set_obj_common_at_info_box(box, rev_head, revision)) return;
    set_at_info_box(box, "Active Program", static_format_obj(rev->active_program.obj, revision));
    set_at_info_box(box, "Vertex Program", static_format_obj(rev->vertex_program.obj, revision));
    set_at_info_box(box, "Fragment Program", static_format_obj(rev->fragment_program.obj, revision));
    set_at_info_box(box, "Geometry Program", static_format_obj(rev->geometry_program.obj, revision));
    set_at_info_box(box, "Tesselation Control Program", static_format_obj(rev->tess_control_program.obj, revision));
    set_at_info_box(box, "Tesselation Eval Program", static_format_obj(rev->tess_eval_program.obj, revision));
    set_at_info_box(box, "Compute Program", static_format_obj(rev->compute_program.obj, revision));
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcProgramPipeline] = &init;
    object_tab_deinit_callbacks[(int)TrcProgramPipeline] = &deinit;
    object_tab_update_callbacks[(int)TrcProgramPipeline] = (object_tab_update_callback_t)&update;
}
