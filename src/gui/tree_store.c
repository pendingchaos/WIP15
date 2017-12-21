#include "tree_store.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct ErrorState {
    bool error;
    bool warning;
    bool info;
} ErrorState;

struct _GuiTreeStore {
    GObject parent_instance;
    int stamp;
    trace_t* trace;
    trace_t* new_trace;
    size_t frame_count;
    ErrorState* frame_errs;
    size_t* cmd_counts;
    size_t** cmds;
    
    GdkPixbuf* error_pixbuf;
    GdkPixbuf* warning_pixbuf;
    GdkPixbuf* info_pixbuf;
    
    GuiTreeStoreFilterFunc filter_func;
    void* filter_udata;
};

typedef struct Iter {
    int depth; //1->points to a frame, 2->points to a command
    size_t frame;
    size_t command;
} Iter;

static trace_t dummy_trace = {.frame_count=0};

static Iter unpack_iter(GtkTreeModel* model, GtkTreeIter* iter) {
    Iter res = {.depth=(uintptr_t)iter->user_data,
                .frame=(uintptr_t)iter->user_data2,
                .command=(uintptr_t)iter->user_data3};
    return res;
}

static GtkTreeIter pack_iter(GtkTreeModel* model, Iter iter) {
    GtkTreeIter res = {.stamp=GUI_TREE_STORE(model)->stamp,
                       .user_data=(gpointer)(uintptr_t)iter.depth,
                       .user_data2=(gpointer)(uintptr_t)iter.frame,
                       .user_data3=(gpointer)(uintptr_t)iter.command};
    return res;
}

static bool iter_valid(GtkTreeModel* model, GtkTreeIter* iter) {
    GuiTreeStore* store = GUI_TREE_STORE(model);
    
    if (iter->stamp != store->stamp) return false;
    Iter i = unpack_iter(model, iter);
    
    if (i.frame >= store->frame_count) return false;
    if (i.depth>1 && i.command>=store->cmd_counts[i.frame])
        return false;
    
    return true;
}

static ErrorState get_command_error_state(trace_command_t* cmd) {
    ErrorState res = {.error=false, .warning=false, .info=false};
    trc_attachment_t* attachment = cmd->attachments;
    while (attachment) {
        res.error = res.error || attachment->type == TrcAttachType_Error;
        res.warning = res.warning || attachment->type == TrcAttachType_Warning;
        res.info = res.info || attachment->type == TrcAttachType_Info;
        attachment = attachment->next;
    }
    return res;
}

static void set_changed(GuiTreeStore* self) {
    GtkTreeModel* model = GTK_TREE_MODEL(self);
    GtkTreePath* path = gtk_tree_path_new_from_indices(0, -1);
    for (size_t i = 0; i < self->frame_count; i++) {
        gtk_tree_path_down(path);
        for (size_t j = 0; j < self->cmd_counts[i]; j++) {
            GtkTreeIter packed = pack_iter(model, (Iter){.depth=2, .frame=i, .command=j});
            gtk_tree_model_row_changed(model, path, &packed);
            gtk_tree_path_next(path);
        }
        gtk_tree_path_up(path);
        
        GtkTreeIter packed = pack_iter(model, (Iter){.depth=1, .frame=i, .command=0});
        gtk_tree_model_row_changed(model, path, &packed);
        gtk_tree_path_next(path);
    }
    gtk_tree_path_free(path);
}

static void rebuild(GuiTreeStore* self) {
    GtkTreeModel* model = GTK_TREE_MODEL(self);
    
    //Run deleted signals
    GtkTreePath* path = gtk_tree_path_new_from_indices(self->frame_count-1, -1);
    for (ptrdiff_t i = self->frame_count-1; i >= 0; i--) {
        if (self->cmd_counts[i] > 0) {
            gtk_tree_path_append_index(path, self->cmd_counts[i]-1);
            for (ptrdiff_t j = self->cmd_counts[i]-1; j >= 0; j--) {
                gtk_tree_model_row_deleted(model, path);
                if (j == 0) {
                    gtk_tree_path_up(path);
                    Iter iter = {.depth=1, .frame=i, .command=0};
                    GtkTreeIter packed = pack_iter(model, iter);
                    gtk_tree_model_row_has_child_toggled(model, path, &packed);
                    gtk_tree_path_down(path);
                }
                gtk_tree_path_prev(path);
                self->cmd_counts[i]--;
            }
            gtk_tree_path_up(path);
        }
        
        gtk_tree_model_row_deleted(model, path);
        gtk_tree_path_prev(path);
    }
    gtk_tree_path_free(path);
    
    //Update trace
    self->trace = self->new_trace ? self->new_trace : &dummy_trace;
    
    self->frame_count = 0;
    self->frame_errs = calloc(self->trace->frame_count, sizeof(ErrorState));
    self->cmd_counts = calloc(self->trace->frame_count, sizeof(size_t));
    self->cmds = calloc(self->trace->frame_count, sizeof(size_t*));
    
    //Run inserted signals and build cmds, cmd_counts and frame_count
    path = gtk_tree_path_new_from_indices(0, -1);
    Iter iter = {.depth=1, .frame=0, .command=0};
    for (iter.frame = 0; iter.frame < self->trace->frame_count; iter.frame++) {
        self->frame_count++;
        self->cmd_counts[iter.frame] = 0;
        self->cmds[iter.frame] = calloc(self->trace->frames[iter.frame].command_count, sizeof(size_t));
        
        GtkTreeIter packed = pack_iter(model, iter);
        gtk_tree_model_row_inserted(model, path, &packed);
        
        trace_frame_t* frame = &self->trace->frames[iter.frame];
        
        ErrorState frame_errs = {.error=false, .warning=false, .info=false};
        
        gtk_tree_path_down(path);
        for (size_t i = 0; i < frame->command_count; i++) {
            bool pass = true;
            if (self->filter_func) {
                pass = self->filter_func(&frame->commands[i], self->filter_udata);
            }
            if (!pass) continue;
            self->cmds[iter.frame][self->cmd_counts[iter.frame]] = i;
            self->cmd_counts[iter.frame]++;
            
            Iter iter2 = {.depth=2, .frame=iter.frame, .command=self->cmd_counts[iter.frame]-1};
            GtkTreeIter packed = pack_iter(model, iter2);
            gtk_tree_model_row_inserted(model, path, &packed);
            
            if (self->cmd_counts[iter.frame] == 1) {
                gtk_tree_path_up(path);
                Iter iter2 = {.depth=1, .frame=iter.frame, .command=0};
                GtkTreeIter packed = pack_iter(model, iter2);
                gtk_tree_model_row_has_child_toggled(model, path, &packed);
                gtk_tree_path_down(path);
            }
            
            gtk_tree_path_next(path);
            
            ErrorState errs = get_command_error_state(&frame->commands[i]);
            frame_errs.error = frame_errs.error || errs.error;
            frame_errs.warning = frame_errs.warning || errs.warning;
            frame_errs.info = frame_errs.info || errs.info;
        }
        gtk_tree_path_up(path);
        
        gtk_tree_path_next(path);
        
        self->frame_errs[iter.frame] = frame_errs;
    }
    gtk_tree_path_free(path);
}

GuiTreeStore* gui_tree_store_new(trace_t* trace) {
    GuiTreeStore* self = (GuiTreeStore*)g_object_new(GUI_TYPE_TREE_STORE, NULL);
    
    do {
        self->stamp = ((gint64)g_random_int()) - G_MININT;
    } while (self->stamp == 0);
    
    self->trace = &dummy_trace;
    self->new_trace = trace;
    self->filter_func = NULL;
    self->filter_udata = NULL;
    self->error_pixbuf = NULL;
    self->warning_pixbuf = NULL;
    self->info_pixbuf = NULL;
    
    rebuild(self);
    
    return self;
}

void gui_tree_store_set_trace(GuiTreeStore* self, trace_t* trace) {
    if (trace == self->trace) return;
    self->new_trace = trace;
    rebuild(self);
}

void gui_tree_store_set_filter_func(GuiTreeStore* self, GuiTreeStoreFilterFunc func, void* udata) {
    self->filter_func = func;
    self->filter_udata = udata;
    rebuild(self);
}

void gui_tree_store_refilter(GuiTreeStore* self) {
    rebuild(self);
}

void gui_tree_store_set_error_pixbuf(GuiTreeStore* self, GdkPixbuf* buf) {
    g_set_object(&GUI_TREE_STORE(self)->error_pixbuf, buf);
    set_changed(self);
}

void gui_tree_store_set_warning_pixbuf(GuiTreeStore* self, GdkPixbuf* buf) {
    g_set_object(&GUI_TREE_STORE(self)->warning_pixbuf, buf);
    set_changed(self);
}

void gui_tree_store_set_info_pixbuf(GuiTreeStore* self, GdkPixbuf* buf) {
    g_set_object(&GUI_TREE_STORE(self)->info_pixbuf, buf);
    set_changed(self);
}

static GtkTreeModelFlags get_flags(GtkTreeModel* self) {
    return 0;
}

static gint get_n_columns(GtkTreeModel* self) {
    return 5;
}

static GType get_column_type(GtkTreeModel* self, gint index) {
    switch (index) {
    case 0: return GDK_TYPE_PIXBUF;
    case 1: return G_TYPE_STRING;
    case 2: return G_TYPE_STRING;
    case 3: return G_TYPE_UINT64;
    case 4: return G_TYPE_UINT64;
    default: return G_TYPE_INVALID;
    }
}

static gboolean get_iter(GtkTreeModel* self, GtkTreeIter* iter, GtkTreePath* path) {
    gint depth = gtk_tree_path_get_depth(path);
    gint* indices = gtk_tree_path_get_indices(path);
    Iter i;
    i.depth = depth;
    if (indices[0] >= GUI_TREE_STORE(self)->frame_count) return FALSE;
    i.frame = indices[0];
    switch (depth) {
    case 1:
        i.command = 0;
        break;
    case 2:
        if (indices[1] >= GUI_TREE_STORE(self)->cmd_counts[indices[0]]) return FALSE;
        i.command = indices[1];
        break;
    default:
        return FALSE;
    }
    *iter = pack_iter(self, i);
    return TRUE;
}

static GtkTreePath* get_path(GtkTreeModel* self, GtkTreeIter* iter) {
    g_return_val_if_fail(iter_valid(self, iter), NULL);
    Iter i = unpack_iter(self, iter);
    if (i.depth == 1)
        return gtk_tree_path_new_from_indices(i.frame, -1);
    else
        return gtk_tree_path_new_from_indices(i.frame, i.command, -1);
}

static void get_value(GtkTreeModel* self, GtkTreeIter* iter, gint column, GValue* value) {
    g_return_if_fail(iter_valid(self, iter));
    GuiTreeStore* store = GUI_TREE_STORE(self);
    
    Iter i = unpack_iter(self, iter);
    
    g_value_init(value, get_column_type(self, column));
    switch (column) {
    case 0: {
        ErrorState errs;
        if (i.depth == 1) { 
            errs = store->frame_errs[i.frame];
        } else {
            uint64_t cmd_index = store->cmds[i.frame][i.command];
            trace_command_t* cmd = store->trace->frames[i.frame].commands + cmd_index;
            errs = get_command_error_state(cmd);
        }
        
        GdkPixbuf* pixbuf = NULL;
        if (errs.error && store->error_pixbuf) pixbuf = store->error_pixbuf;
        else if (errs.warning && store->warning_pixbuf) pixbuf = store->warning_pixbuf;
        else if (errs.info && store->info_pixbuf) pixbuf = store->info_pixbuf;
        g_value_set_object(value, pixbuf);
        break;
    }
    case 1: {
        char rev_str[64] = {0};
        if (i.depth == 2) {
            uint64_t cmd_index = store->cmds[i.frame][i.command];
            trace_command_t* cmd = store->trace->frames[i.frame].commands + cmd_index;
            sprintf(rev_str, "%lu", cmd->revision);
        }
        g_value_set_string(value, rev_str);
        break;
    }
    case 2: {
        char str[1024] = {0};
        if (i.depth == 2) {
            uint64_t cmd_index = store->cmds[i.frame][i.command];
            trace_command_t* cmd = store->trace->frames[i.frame].commands + cmd_index;
            format_command(store->trace, str, cmd, 1024);
        } else {
            sprintf(str, "Frame %zu", i.frame);
        }
        g_value_set_string(value, str);
        break;
    }
    case 3: {
        g_value_set_uint64(value, i.frame);
        break;
    }
    case 4: {
        g_value_set_uint64(value, i.depth==1?0:i.command);
        break;
    }
    }
}

static gboolean iter_next(GtkTreeModel* self, GtkTreeIter* iter) {
    g_return_val_if_fail(iter_valid(self, iter), FALSE);
    
    iter->stamp = 0;
    Iter i = unpack_iter(self, iter);
    switch (i.depth) {
    case 1: {
        i.frame++;
        if (i.frame >= GUI_TREE_STORE(self)->frame_count)
            return FALSE;
        *iter = pack_iter(self, i);
        return TRUE;
    }
    case 2: {
        i.command++;
        if (i.command >= GUI_TREE_STORE(self)->cmd_counts[i.frame])
            return FALSE;
        *iter = pack_iter(self, i);
        return TRUE;
    }
    }
    return FALSE;
}

static gboolean iter_previous(GtkTreeModel* self, GtkTreeIter* iter) {
    g_return_val_if_fail(iter_valid(self, iter), FALSE);
    
    Iter i = unpack_iter(self, iter);
    iter->stamp = 0;
    switch (i.depth) {
    case 1: {
        if (i.frame == 0) return FALSE;
        i.frame--;
        *iter = pack_iter(self, i);
        return TRUE;
    }
    case 2: {
        if (i.command==0 && i.frame==0)
            return FALSE;
        else if (i.command == 0)
            return FALSE;
        else
            i.command--;
        break;
    }
    default: {
        return FALSE;
    }
    }
    *iter = pack_iter(self, i);
    return TRUE;
}

static gboolean iter_children(GtkTreeModel* self, GtkTreeIter* iter, GtkTreeIter* parent) {
    iter->stamp = 0;
    
    if (parent == NULL) {
        if (GUI_TREE_STORE(self)->frame_count == 0)
            return FALSE;
        Iter i = {.depth=1, .frame=0};
        *iter = pack_iter(self, i);
        return TRUE;
    }
    
    g_return_val_if_fail(iter_valid(self, parent), FALSE);
    
    Iter pi = unpack_iter(self, parent);
    if (pi.depth == 2) return FALSE;
    if (GUI_TREE_STORE(self)->cmd_counts[pi.frame] == 0)
        return FALSE;
    
    Iter i;
    i.depth = 2;
    i.frame = pi.frame;
    i.command = 0;
    *iter = pack_iter(self, i);
    
    return TRUE;
}

static gboolean iter_has_child(GtkTreeModel* self, GtkTreeIter* iter) {
    g_return_val_if_fail(iter_valid(self, iter), FALSE);
    
    Iter i = unpack_iter(self, iter);
    if (i.depth != 1) return FALSE;
    return GUI_TREE_STORE(self)->cmd_counts[i.frame] > 0;
}

static gint iter_n_children(GtkTreeModel* self, GtkTreeIter* iter) {
    if (iter == NULL) return GUI_TREE_STORE(self)->frame_count;
    g_return_val_if_fail(iter_valid(self, iter), 0);
    Iter i = unpack_iter(self, iter);
    if (i.depth == 1)
        return GUI_TREE_STORE(self)->cmd_counts[i.frame];
    return 0;
}

static gboolean iter_nth_child(GtkTreeModel* self, GtkTreeIter* iter, GtkTreeIter* parent, gint n) {
    iter->stamp = 0;
    
    if (parent == NULL) {
        if (n >= GUI_TREE_STORE(self)->frame_count) return FALSE;
        Iter i;
        i.depth = 1;
        i.frame = n;
        i.command = 0;
        *iter = pack_iter(self, i);
        return TRUE;
    }
    
    g_return_val_if_fail(iter_valid(self, iter), FALSE);
    
    Iter pi = unpack_iter(self, parent);
    if (pi.depth != 1) return FALSE;
    
    Iter i;
    i.depth = 2;
    i.frame = pi.frame;
    i.command = n;
    
    if (i.command >= GUI_TREE_STORE(self)->cmd_counts[i.frame])
        return FALSE;
    
    *iter = pack_iter(self, i);
    return TRUE;
}

static gboolean iter_parent(GtkTreeModel* self, GtkTreeIter* iter, GtkTreeIter* child) {
    iter->stamp = 0;
    
    g_return_val_if_fail(iter_valid(self, child), FALSE);
    
    Iter i = unpack_iter(self, child);
    if (i.depth == 1) return FALSE;
    i.depth = 1;
    i.command = 0;
    *iter = pack_iter(self, i);
    return TRUE;
}

static void ref_node(GtkTreeModel* self, GtkTreeIter* iter) {}

static void unref_node(GtkTreeModel* self, GtkTreeIter* iter) {}

static void gui_tree_store_interface_init(GtkTreeModelIface* iface) {
    iface->get_flags = get_flags;
    iface->get_n_columns = get_n_columns;
    iface->get_column_type = get_column_type;
    iface->get_iter = get_iter;
    iface->get_path = get_path;
    iface->get_value = get_value;
    iface->iter_next = iter_next;
    iface->iter_previous = iter_previous;
    iface->iter_children = iter_children;
    iface->iter_has_child = iter_has_child;
    iface->iter_n_children = iter_n_children;
    iface->iter_nth_child = iter_nth_child;
    iface->iter_parent = iter_parent;
    iface->ref_node = ref_node;
    iface->unref_node = unref_node;
}

G_DEFINE_TYPE_WITH_CODE(GuiTreeStore, gui_tree_store, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(GTK_TYPE_TREE_MODEL,
                                              gui_tree_store_interface_init))

static void dispose(GObject* gobject) {
    GuiTreeStore* store = GUI_TREE_STORE(gobject);
    g_clear_object(&store->error_pixbuf);
    g_clear_object(&store->warning_pixbuf);
    g_clear_object(&store->info_pixbuf);
}

static void finalize(GObject* gobject) {
    G_OBJECT_CLASS(gui_tree_store_parent_class)->finalize(gobject);
}

static void gui_tree_store_class_init(GuiTreeStoreClass* klass) {
    GObjectClass* obj_class = G_OBJECT_CLASS(klass);
    obj_class->finalize = finalize;
    obj_class->dispose = dispose;
}

static void gui_tree_store_init(GuiTreeStore* self) {
    //Everything is done in gui_tree_store_new()
}
