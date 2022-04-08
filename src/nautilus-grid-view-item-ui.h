#pragma once

#include <glib.h>
#include <gtk/gtk.h>

#include "nautilus-view-item-model.h"

G_BEGIN_DECLS

enum
{
    NAUTILUS_VIEW_ICON_FIRST_CAPTION,
    NAUTILUS_VIEW_ICON_SECOND_CAPTION,
    NAUTILUS_VIEW_ICON_THIRD_CAPTION,
    NAUTILUS_VIEW_ICON_N_CAPTIONS
};

#define NAUTILUS_TYPE_GRID_VIEW_ITEM_UI (nautilus_grid_view_item_ui_get_type())

G_DECLARE_FINAL_TYPE (NautilusGridViewItemUi, nautilus_grid_view_item_ui, NAUTILUS, GRID_VIEW_ITEM_UI, GtkBox)

NautilusGridViewItemUi * nautilus_grid_view_item_ui_new (void);
void nautilus_grid_view_item_ui_set_model (NautilusGridViewItemUi *self,
                                           NautilusViewItemModel  *model);
NautilusViewItemModel *nautilus_grid_view_item_ui_get_model (NautilusGridViewItemUi *self);
void nautilus_view_item_ui_set_caption_attributes (NautilusGridViewItemUi *self,
                                                   GQuark                 *attrs);
gboolean nautilus_grid_view_item_ui_once (NautilusGridViewItemUi *self);

G_END_DECLS
