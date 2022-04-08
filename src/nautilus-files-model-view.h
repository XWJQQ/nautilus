/* nautilus-files-model-view.h
 *
 * Header of abstract type, to be considered private to its subclasses.
 * */

#pragma once

#include <glib.h>
#include <gtk/gtk.h>

#include "nautilus-files-view.h"
#include "nautilus-view-model.h"

G_BEGIN_DECLS

#define NAUTILUS_TYPE_FILES_MODEL_VIEW (nautilus_files_model_view_get_type())

G_DECLARE_DERIVABLE_TYPE (NautilusFilesModelView, nautilus_files_model_view, NAUTILUS, FILES_MODEL_VIEW, NautilusFilesView)

struct _NautilusFilesModelViewClass
{
        NautilusFilesViewClass parent_class;

        guint      (*get_icon_size)  (NautilusFilesModelView *self);
        GtkWidget *(*get_view_ui)    (NautilusFilesModelView *self);
        void       (*scroll_to_item) (NautilusFilesModelView *self,
                                      guint                   position);
};

/* Methods */
NautilusViewModel *nautilus_files_model_view_get_model     (NautilusFilesModelView *self);
void               nautilus_files_model_view_set_icon_size (NautilusFilesModelView *self,
                                                            gint                    icon_size);

/* Shareable helpers */
void                          set_directory_sort_metadata       (NautilusFile *file,
                                                                 const gchar  *metadata_name,
                                                                 gboolean      reversed);
const NautilusFileSortType    get_sorts_type_from_metadata_text (const char   *metadata_name);

#define listitem_get_view_item(li) \
(NAUTILUS_VIEW_ITEM_MODEL (gtk_tree_list_row_get_item (GTK_TREE_LIST_ROW (gtk_list_item_get_item (li)))))

G_END_DECLS
