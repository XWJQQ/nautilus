#pragma once

#include <glib.h>
#include <gtk/gtk.h>

#include "nautilus-files-model-view.h"
#include "nautilus-window-slot.h"

G_BEGIN_DECLS

#define NAUTILUS_TYPE_GRID_VIEW (nautilus_grid_view_get_type())

G_DECLARE_FINAL_TYPE (NautilusGridView, nautilus_grid_view, NAUTILUS, GRID_VIEW, NautilusFilesModelView)

NautilusGridView *nautilus_grid_view_new (NautilusWindowSlot *slot);

G_END_DECLS
