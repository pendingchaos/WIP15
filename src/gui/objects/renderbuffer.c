#include "../gui.h"
#include "../utils.h"

static void init(object_tab_t* tab) {
    add_obj_common_to_info_box(tab->info_box);
    add_custom_to_info_box(tab->info_box, "No storage allocated", NULL);
    add_to_info_box(tab->info_box, "Width");
    add_to_info_box(tab->info_box, "Height");
    add_to_info_box(tab->info_box, "Internal Format");
    add_to_info_box(tab->info_box, "Sample Count");
    add_to_info_box(tab->info_box, "Size (bits)");
}

static void deinit(object_tab_t* tab) {}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_renderbuffer_rev_t* rev = (const trc_gl_renderbuffer_rev_t*)rev_head;
    
    info_box_t* box = tab->info_box;
    if (!set_obj_common_at_info_box(box, rev_head, revision)) return;
    set_visible_at_info_box(box, "No storage allocated", !rev->has_storage);
    set_visible_at_info_box(box, "Width", rev->has_storage);
    set_visible_at_info_box(box, "Height", rev->has_storage);
    set_visible_at_info_box(box, "Internal Format", rev->has_storage);
    set_visible_at_info_box(box, "Sample Count", rev->has_storage);
    set_visible_at_info_box(box, "Size (bits)", rev->has_storage);
    if (rev->has_storage) {
        set_at_info_box(box, "Width", "%u", rev->width);
        set_at_info_box(box, "Height", "%u", rev->width);
        set_enum_at_info_box(box, "Internal Format", "InternalFormat", rev->internal_format);
        set_at_info_box(box, "Sample Count", "%u", rev->sample_count);
        if (rev->rgba_bits[0])
            set_at_info_box(box, "Size (bits)", "[%u, %u, %u, %u]",
                            rev->rgba_bits[0], rev->rgba_bits[1], rev->rgba_bits[2], rev->rgba_bits[3]);
        else if (rev->depth_bits && rev->stencil_bits)
            set_at_info_box(box, "Size (bits)", "D:%u S:%u", rev->depth_bits, rev->stencil_bits);
        else if (rev->depth_bits)
            set_at_info_box(box, "Size (bits)", "%u", rev->depth_bits);
        else if (rev->stencil_bits)
            set_at_info_box(box, "Size (bits)", "%u", rev->stencil_bits);
    }
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcRenderbuffer] = &init;
    object_tab_deinit_callbacks[(int)TrcRenderbuffer] = &deinit;
    object_tab_update_callbacks[(int)TrcRenderbuffer] = (object_tab_update_callback_t)&update;
}
