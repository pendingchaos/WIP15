#include "../gui.h"
#include "../utils.h"

#include <stdlib.h>
#include <string.h>

typedef struct program_data_t {
    GtkTextView* info_log;
    GtkTextView* source;
} program_data_t;

static void init(object_tab_t* tab) {
    program_data_t* data = malloc(sizeof(program_data_t));
    tab->data = data;
    add_obj_common_to_info_box(tab->info_box);
    add_to_info_box(tab->info_box, "Type");
    
    add_separator_to_info_box(tab->info_box);
    add_custom_to_info_box(tab->info_box, "Source", NULL);
    
    data->source = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(data->source, false);
    gtk_widget_set_vexpand(GTK_WIDGET(data->source), true);
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(data->source)));
    
    add_separator_to_info_box(tab->info_box);
    add_custom_to_info_box(tab->info_box, "Info Log", NULL);
    
    data->info_log = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(data->info_log, false);
    gtk_widget_set_vexpand(GTK_WIDGET(data->info_log), true);
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(data->info_log)));
}

static void deinit(object_tab_t* tab) {
    free(tab->data);
}

static char* get_shader_source(const trc_gl_shader_rev_t* shdr) {
    char* source = calloc(shdr->sources->size==0?1:shdr->sources->size, 1);
    const char* sourcesource = trc_map_data(shdr->sources, TRC_MAP_READ);
    for (size_t i = 0; i < shdr->sources->size; i++) {
        if (sourcesource[i] != 0)
            source[strlen(source)] = sourcesource[i];
    }
    trc_unmap_data(sourcesource);
    
    return source;
}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_shader_rev_t* rev = (const trc_gl_shader_rev_t*)rev_head;
    program_data_t* data = tab->data;
    
    if (!set_obj_common_at_info_box(tab->info_box, rev_head, revision)) return;
    set_enum_at_info_box(tab->info_box, "Type", "ShaderType", rev->type);
    
    //Info log
    const char* info_log = trc_map_data(rev->info_log, TRC_MAP_READ);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(data->info_log), info_log, -1);
    trc_unmap_data(info_log);
    
    //Source
    char* source = get_shader_source(rev);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(data->source), source, -1);
    free(source);
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcShader] = &init;
    object_tab_deinit_callbacks[(int)TrcShader] = &deinit;
    object_tab_update_callbacks[(int)TrcShader] = (object_tab_update_callback_t)&update;
}
