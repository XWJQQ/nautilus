#pragma once

#include <glib.h>
#include <gtk/gtk.h>

#include "nautilus-view-item-model.h"

G_BEGIN_DECLS

#define NAUTILUS_TYPE_LIST_VIEW_ITEM_UI (nautilus_list_view_item_ui_get_type())

G_DECLARE_FINAL_TYPE (NautilusListViewItemUi, nautilus_list_view_item_ui, NAUTILUS, LIST_VIEW_ITEM_UI, GtkBox)

NautilusListViewItemUi * nautilus_list_view_item_ui_new (void);
void nautilus_list_view_item_ui_set_model (NautilusListViewItemUi *self,
                                           NautilusViewItemModel  *model);
NautilusViewItemModel *nautilus_list_view_item_ui_get_model (NautilusListViewItemUi *self);
void nautilus_list_view_item_ui_set_path (NautilusListViewItemUi *self,
                                          GQuark                  path_attribute_q,
                                          GFile                  *base_location);
void nautilus_list_view_item_ui_show_snippet (NautilusListViewItemUi *self);
gboolean nautilus_list_view_item_ui_once (NautilusListViewItemUi *self);

G_END_DECLS
