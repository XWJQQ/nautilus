#pragma once

#include <glib.h>
#include <gtk/gtk.h>

#include "nautilus-files-model-view.h"
#include "nautilus-window-slot.h"
#include "nautilus-view-model.h"

G_BEGIN_DECLS

#define NAUTILUS_TYPE_LIST_VIEW (nautilus_list_view_get_type())

G_DECLARE_FINAL_TYPE (NautilusListView, nautilus_list_view, NAUTILUS, LIST_VIEW, NautilusFilesModelView)

NautilusListView *nautilus_list_view_new (NautilusWindowSlot *slot);

G_END_DECLS
