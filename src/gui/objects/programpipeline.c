#include "../gui.h"
#include "../utils.h"

typedef struct program_pipeline_data_t {
    object_button_t* buttons[7];
} program_pipeline_data_t;

static void init(object_tab_t* tab) {
    program_pipeline_data_t* data = malloc(sizeof(program_pipeline_data_t));
    tab->data = data;
    add_obj_common_to_info_box(tab->info_box);
    
    const char* names[] = {"Active Program", "Vertex Program",
        "Fragment Program", "Geometry Program", "Tesselation Control Program",
        "Tesselation Evaluation Program", "Compute Program"};
    for (size_t i = 0; i < 7; i++) {
        data->buttons[i] = create_object_button();
        add_custom_to_info_box(tab->info_box, names[i], data->buttons[i]->widget);
    }
}

static void deinit(object_tab_t* tab) {}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_program_pipeline_rev_t* rev = (const trc_gl_program_pipeline_rev_t*)rev_head;
    
    program_pipeline_data_t* data = tab->data;
    info_box_t* box = tab->info_box;
    if (!set_obj_common_at_info_box(box, rev_head, revision)) return;
    update_object_button(data->buttons[0], rev->active_program.obj, revision);
    update_object_button(data->buttons[1], rev->vertex_program.obj, revision);
    update_object_button(data->buttons[2], rev->fragment_program.obj, revision);
    update_object_button(data->buttons[3], rev->geometry_program.obj, revision);
    update_object_button(data->buttons[4], rev->tess_control_program.obj, revision);
    update_object_button(data->buttons[5], rev->tess_eval_program.obj, revision);
    update_object_button(data->buttons[6], rev->compute_program.obj, revision);
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcProgramPipeline] = &init;
    object_tab_deinit_callbacks[(int)TrcProgramPipeline] = &deinit;
    object_tab_update_callbacks[(int)TrcProgramPipeline] = (object_tab_update_callback_t)&update;
}
