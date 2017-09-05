#include "libtrace/libtrace.h"
#include "../gui.h"
#include "object.h"

#include <gtk/gtk.h>
#include <stdlib.h>

object_tab_callback_t object_tab_init_callbacks[(int)Trc_ObjMax] = {0};
object_tab_callback_t object_tab_deinit_callbacks[(int)Trc_ObjMax] = {0};
object_tab_update_callback_t object_tab_update_callbacks[(int)Trc_ObjMax] = {0};

static bool parse_uint(const char* text, int64_t* val) {
    *val = 0;
    if (*text == 0) return false;
    for (const char* c = text; *c; c++) {
        if (*c>='0' && *c<='9')
            *val = (*val)*10 + *c-'0';
        else
            return false;
    }
    return true;
}

static void object_tab_deinit(gui_tab_t* gtab) {
    object_tab_t* otab = gtab->data;
    object_tab_callback_t cb = object_tab_deinit_callbacks[(int)otab->type];
    if (cb) cb(otab);
    destroy_object_notebook(otab->obj_notebook);
    free_info_box(otab->info_box);
    free(otab);
}

static void object_tab_update(gui_tab_t* gtab) {
    object_tab_t* otab = gtab->data;
    
    static const char* names[] = {
        [TrcBuffer] = "Buffer ",
        [TrcSampler] = "Sampler ",
        [TrcTexture] = "Texture ",
        [TrcQuery] = "Query ",
        [TrcFramebuffer] = "Framebuffer ",
        [TrcRenderbuffer] = "Renderbuffer ",
        [TrcSync] = "Sync ",
        [TrcProgram] = "Program ",
        [TrcProgramPipeline] = "Program Pipeline ",
        [TrcShader] = "Shader ",
        [TrcVAO] = "VAO ",
        [TrcTransformFeedback] = "Transform Feedback ",
        [TrcContext] = "Context "
    };
    
    const char* problem_text = NULL;
    
    int64_t revision = state.revision;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(otab->revision_checkbox))) {
        const char* text = gtk_entry_get_text(GTK_ENTRY(otab->revision_entry));
        if (!parse_uint(text, &revision)) {
            problem_text = static_format("Invalid revision");
            revision = -1;
        }
    } else {
        char rev_text[64];
        sprintf(rev_text, "%ld", state.revision);
        gtk_entry_set_text(GTK_ENTRY(otab->revision_entry), rev_text);
    }
    
    const trc_obj_rev_head_t* rev = NULL;
    if (revision >= 0) rev = trc_obj_get_rev(otab->obj, revision);
    
    if (!rev && !problem_text) {
        problem_text = static_format(
            "This object was only created at revision %lu",
            ((trc_obj_rev_head_t*)otab->obj->revisions[0])->revision);
    }
    if (rev && rev->ref_count==0)
        problem_text = static_format("This object has been deleted at revision %lu", rev->revision);
    
    if (problem_text)
        gtk_label_set_text(GTK_LABEL(otab->problem_label), problem_text);
    
    gtk_widget_set_visible(otab->problem_label, problem_text!=NULL);
    gtk_widget_set_visible(otab->obj_notebook->widget, problem_text==NULL);
    
    const char* text = static_format("%s %s", names[otab->type], static_format_obj(otab->obj, revision));
    gtk_label_set_text(GTK_LABEL(gtab->tab_label), text);
    
    object_tab_update_callback_t cb = object_tab_update_callbacks[(int)otab->type];
    if (rev && cb) cb(otab, rev, revision);
}

static void revision_checkbox_callback(GtkToggleButton* togglebutton, object_tab_t* tab) {
    bool sensitive = gtk_toggle_button_get_active(togglebutton);
    gtk_widget_set_sensitive(tab->revision_entry, sensitive);
    update_tab(tab->tab);
}

static void revision_entry_callback(GtkEntry* entry, object_tab_t* tab) {
    update_tab(tab->tab);
}

gui_tab_t* open_object_tab(trc_obj_t* obj) {
    object_tab_t* tab = malloc(sizeof(object_tab_t));
    tab->tab = NULL;
    tab->obj = obj;
    tab->type = obj->type;
    tab->data = NULL;
    
    tab->obj_notebook = create_object_notebook();
    tab->info_box = create_info_box();
    add_object_notebook_tab(tab->obj_notebook, "Main", false, tab->info_box->widget, NULL, NULL);
    
    object_tab_callback_t cb = object_tab_init_callbacks[(int)tab->type];
    if (cb) {
        cb(tab);
    } else {
        GtkWidget* widget = gtk_image_new_from_icon_name("broken", GTK_ICON_SIZE_DIALOG);
        add_custom_to_info_box(tab->info_box, NULL, widget);
    }
    
    tab->revision_checkbox = gtk_check_button_new_with_label("Custom Revision");
    tab->revision_entry = gtk_entry_new();
    gtk_widget_set_sensitive(tab->revision_entry, false);
    gtk_entry_set_input_purpose(GTK_ENTRY(tab->revision_entry), GTK_INPUT_PURPOSE_DIGITS);
    
    g_signal_connect(tab->revision_checkbox, "toggled", G_CALLBACK(revision_checkbox_callback), tab);
    g_signal_connect(tab->revision_entry, "activate", G_CALLBACK(revision_entry_callback), tab);
    
    tab->revision_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(tab->revision_box), tab->revision_checkbox, false, false, 0);
    gtk_box_pack_start(GTK_BOX(tab->revision_box), tab->revision_entry, true, true, 0);
    
    tab->problem_label = gtk_label_new("");
    gtk_widget_set_visible(tab->problem_label, false);
    gtk_widget_set_visible(tab->obj_notebook->widget, false);
    
    GtkWidget* content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(content), tab->revision_box, false, false, 0);
    gtk_box_pack_start(GTK_BOX(content), tab->problem_label, true, false, 0);
    GtkWidget* notebook_window = create_scrolled_window(tab->obj_notebook->widget);
    gtk_box_pack_start(GTK_BOX(content), notebook_window, true, true, 0);
    
    tab->tab = open_gui_tab(true, content);
    tab->tab->data = tab;
    tab->tab->deinit = &object_tab_deinit;
    tab->tab->update = &object_tab_update;
    
    update_tab(tab->tab);
    
    return tab->tab;
}
