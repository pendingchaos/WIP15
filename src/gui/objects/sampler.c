#include "../gui.h"
#include "../utils.h"

static void init(object_tab_t* tab) {
    add_obj_common_to_info_box(tab->info_box);
    
    add_multiple_to_info_box(tab->info_box,
        "Minification Filter", "Magnification Filter", "Minimum LOD",
        "Maximum LOD", "Wrap S", "Wrap T", "Wrap R", "Max Anisotropy",
        "Border Color", "Compare Mode", "Compare Function", "LOD Bias", NULL);
}

static void deinit(object_tab_t* tab) {}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    trc_gl_sample_params_t params = ((const trc_gl_sampler_rev_t*)rev_head)->params;
    
    info_box_t* box = tab->info_box;
    if (!set_obj_common_at_info_box(box, rev_head, revision)) return;
    set_enum_at_info_box(box, "Minification Filter", "TextureMinFilter", params.min_filter);
    set_enum_at_info_box(box, "Magnification Filter", "TextureMagFilter", params.mag_filter);
    set_at_info_box(box, "Minimum LOD", "%g", params.min_lod);
    set_at_info_box(box, "Maximum LOD", "%g", params.max_lod);
    set_enum_at_info_box(box, "Wrap S", "TextureWrapMode", params.wrap_s);
    set_enum_at_info_box(box, "Wrap T", "TextureWrapMode", params.wrap_t);
    set_enum_at_info_box(box, "Wrap R", "TextureWrapMode", params.wrap_r);
    set_at_info_box(box, "Max Anisotropy", "%g", params.max_anisotropy);
    set_at_info_box(box, "Border Color", "[%g, %g, %g, %g]",
                    params.border_color[0], params.border_color[1],
                    params.border_color[2], params.border_color[3]);
    set_enum_at_info_box(box, "Compare Mode", "TexCompareMode", params.compare_mode);
    set_enum_at_info_box(box, "Compare Function", "DepthFunction", params.compare_func);
    set_at_info_box(box, "LOD Bias", "%g", params.lod_bias);
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcSampler] = &init;
    object_tab_deinit_callbacks[(int)TrcSampler] = &deinit;
    object_tab_update_callbacks[(int)TrcSampler] = (object_tab_update_callback_t)&update;
}
