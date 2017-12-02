#include "object_button.h"
#include "../utils.h"
#include "../tabs/object.h"

static void destroyed(GtkWidget* _, object_button_t* button) {
    free(button);
}

static gboolean clicked(GtkWidget* _, object_button_t* button) {
    if (button->object) open_object_tab(button->object, button->revision);
    return TRUE;
}

static gboolean handle_event(GtkWidget* widget, GdkEvent* event, gpointer user_data) {
    switch (event->type) {
    case GDK_ENTER_NOTIFY:
    case GDK_LEAVE_NOTIFY: {
        GdkDisplay *display = gtk_widget_get_display(widget);
        GdkCursor *cursor = NULL;
        if (event->type == GDK_ENTER_NOTIFY)
            cursor = gdk_cursor_new_from_name(display, "pointer");
        gdk_window_set_cursor(gtk_widget_get_window(widget), cursor);
        gdk_display_flush(display);
        if (cursor) g_object_unref(cursor);
        return TRUE;
    }
    default: {
        return FALSE;
    }
    }
}

object_button_t* create_object_button() {
    object_button_t* button = malloc(sizeof(object_button_t*));
    
    button->widget = gtk_button_new();
    button->object = NULL;
    
    GtkWidget* widget = button->widget;
    gtk_button_set_relief(GTK_BUTTON(widget), GTK_RELIEF_NONE);
    gtk_widget_set_state_flags(widget, GTK_STATE_FLAG_LINK, FALSE);
    gtk_widget_set_focus_on_click(widget, FALSE);
    gint events = GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK;
    gtk_widget_add_events(widget, events);
    g_signal_connect(widget, "clicked", G_CALLBACK(clicked), button);
    g_signal_connect(widget, "event", G_CALLBACK(handle_event), NULL);
    g_signal_connect(widget, "destroy", G_CALLBACK(destroyed), button);
    
    GtkStyleContext* context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(context, "object_button");
    
    return button;
}

void update_object_button(object_button_t* button, trc_obj_t* obj, uint64_t revision) {
    button->object = obj;
    button->revision = revision;
    
    const char* label = static_format_obj(obj, revision);
    gtk_button_set_label(GTK_BUTTON(button->widget), label);
    
    GtkBin* bin = GTK_BIN(button->widget);
    gtk_widget_set_halign(gtk_bin_get_child(bin), GTK_ALIGN_START);
}
