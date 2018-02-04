#include "gui.h"

#define WIDGETS \
    GtkComboBox* thread_count_mode = GTK_COMBO_BOX(\
        gtk_builder_get_object(state.prefs_builder, "thread_count_mode"));\
    GtkSpinButton* thread_count = GTK_SPIN_BUTTON(\
        gtk_builder_get_object(state.prefs_builder, "thread_count"));\
    \
    GtkSwitch* lz4_enabled = GTK_SWITCH(\
        gtk_builder_get_object(state.prefs_builder, "lz4_enabled"));\
    GtkSwitch* zlib_enabled = GTK_SWITCH(\
        gtk_builder_get_object(state.prefs_builder, "zlib_enabled"));\
    GtkSwitch* zstd_enabled = GTK_SWITCH(\
        gtk_builder_get_object(state.prefs_builder, "zstd_enabled"));\
    \
    GtkRange* lz4_level = GTK_RANGE(\
        gtk_builder_get_object(state.prefs_builder, "lz4_level"));\
    GtkRange* zlib_level = GTK_RANGE(\
        gtk_builder_get_object(state.prefs_builder, "zlib_level"));\
    GtkRange* zstd_level = GTK_RANGE(\
        gtk_builder_get_object(state.prefs_builder, "zstd_level"));

static void on_prefs_update();

void fill_prefs_window() {
    WIDGETS
    
    trc_data_settings_t s = state.data_settings;
    
    switch (s.thread_count) {
    case 0: gtk_combo_box_set_active(thread_count_mode, 1); break;
    case -1: gtk_combo_box_set_active(thread_count_mode, 0); break;
    default: {
        gtk_combo_box_set_active(thread_count_mode, 2);
        gtk_spin_button_set_value(thread_count, s.thread_count);
        break;
    }
    }
    
    gtk_switch_set_active(lz4_enabled, s.lz4_enabled);
    gtk_switch_set_active(zlib_enabled, s.zlib_enabled);
    gtk_switch_set_active(zstd_enabled, s.zstd_enabled);
    
    gtk_range_set_value(lz4_level, 50-s.lz4_acceleration);
    gtk_range_set_value(zlib_level, s.zlib_level);
    gtk_range_set_value(zstd_level, s.zstd_level);
    
    GtkAdjustment* adj = gtk_spin_button_get_adjustment(thread_count);
    gtk_adjustment_set_upper(adj, sysconf(_SC_NPROCESSORS_ONLN)*2);
    
    bool avail = trc_compression_available(TrcCompression_LZ4);
    gtk_widget_set_sensitive(GTK_WIDGET(lz4_enabled), avail);
    
    avail = trc_compression_available(TrcCompression_Zlib);
    gtk_widget_set_sensitive(GTK_WIDGET(zlib_enabled), avail);
    
    avail = trc_compression_available(TrcCompression_Zstd);
    gtk_widget_set_sensitive(GTK_WIDGET(zstd_enabled), avail);
}

static void on_prefs_update() {
    WIDGETS
    
    trc_data_settings_t* s = &state.data_settings;
    //Update s
    switch (gtk_combo_box_get_active(thread_count_mode)) {
    case 0: s->thread_count = -1; break;
    case 1: s->thread_count = 0; break;
    case 2: s->thread_count = gtk_spin_button_get_value(thread_count); break;
    }
    
    s->lz4_enabled = gtk_switch_get_state(lz4_enabled);
    s->zlib_enabled = gtk_switch_get_state(zlib_enabled);
    s->zstd_enabled = gtk_switch_get_state(zstd_enabled);
    
    s->lz4_acceleration = 50 - gtk_range_get_value(lz4_level);
    s->zlib_level = gtk_range_get_value(zlib_level);
    s->zstd_level = gtk_range_get_value(zstd_level);
    
    //Update view
    bool thread_count_sensitive = gtk_combo_box_get_active(thread_count_mode) == 2;
    gtk_widget_set_sensitive(GTK_WIDGET(thread_count), thread_count_sensitive);
    
    bool avail = trc_compression_available(TrcCompression_LZ4);
    gtk_widget_set_sensitive(GTK_WIDGET(lz4_level), s->lz4_enabled&&avail);
    
    avail = trc_compression_available(TrcCompression_Zlib);
    gtk_widget_set_sensitive(GTK_WIDGET(zlib_level), s->zlib_enabled&&avail);
    
    avail = trc_compression_available(TrcCompression_Zstd);
    gtk_widget_set_sensitive(GTK_WIDGET(zstd_level), s->zstd_enabled&&avail);
    
    state.pref_file_dirty = true;
}

VISIBLE gboolean prefs_state_set(GtkSwitch* sw, gboolean state, gpointer user_data) {
    gtk_switch_set_state(sw, state);
    on_prefs_update();
    return FALSE;
}

VISIBLE void prefs_value_changed(GObject* obj, gpointer user_data) {
    on_prefs_update();
}
