#include "nautilus-list-view-item-ui.h"
#include "nautilus-view-item-model.h"
#include "nautilus-file.h"
#include "nautilus-file-utilities.h"
#include "nautilus-thumbnails.h"

struct _NautilusListViewItemUi
{
    GtkBox parent_instance;

    NautilusViewItemModel *model;
    GQuark path_attribute_q;
    GFile *file_path_base_location;

    GtkWidget *fixed_height_box;
    GtkWidget *icon;
    GtkWidget *label;
    GtkWidget *snippet;
    GtkWidget *path;

    gboolean show_snippet;
    gboolean called_once;
};

G_DEFINE_TYPE (NautilusListViewItemUi, nautilus_list_view_item_ui, GTK_TYPE_BOX)

enum
{
    PROP_0,
    PROP_MODEL,
    N_PROPS
};

static void
update_icon (NautilusListViewItemUi *self)
{
    NautilusFileIconFlags flags;
    g_autoptr (GdkPaintable) icon_paintable = NULL;
    GtkStyleContext *style_context;
    NautilusFile *file;
    guint icon_size;
    int icon_height;
    int extra_margin;
    g_autofree gchar *thumbnail_path = NULL;

    file = nautilus_view_item_model_get_file (self->model);
    icon_size = nautilus_view_item_model_get_icon_size (self->model);
    flags = NAUTILUS_FILE_ICON_FLAGS_USE_THUMBNAILS |
            NAUTILUS_FILE_ICON_FLAGS_FORCE_THUMBNAIL_SIZE |
            NAUTILUS_FILE_ICON_FLAGS_USE_EMBLEMS |
            NAUTILUS_FILE_ICON_FLAGS_USE_ONE_EMBLEM;

    icon_paintable = nautilus_file_get_icon_paintable (file, icon_size, 1, flags);
    gtk_picture_set_paintable (GTK_PICTURE (self->icon), icon_paintable);

    /* Set the same width for all icons regardless of aspect ratio.
     * Don't set the width here because it would get GtkPicture w4h confused.
     */
    gtk_widget_set_size_request (self->fixed_height_box, icon_size, -1);

    /* Give all items the same minimum width. This cannot be done by setting the
     * width request directly, as above, because it would get mess up with
     * height for width calculations.
     *
     * Instead we must add margins on both sides of the icon which, summed up
     * with the icon's actual width, equal the desired item width. */
    icon_height = gdk_paintable_get_intrinsic_height (icon_paintable);
    extra_margin = (icon_size - icon_height) / 2;
    gtk_widget_set_margin_top (self->fixed_height_box, extra_margin);
    gtk_widget_set_margin_bottom (self->fixed_height_box, extra_margin);

    style_context = gtk_widget_get_style_context (self->icon);
    thumbnail_path = nautilus_file_get_thumbnail_path (file);
    if (icon_size >= NAUTILUS_THUMBNAIL_MINIMUM_ICON_SIZE &&
        thumbnail_path != NULL &&
        nautilus_file_should_show_thumbnail (file))
    {
        gtk_style_context_add_class (style_context, "thumbnail");
    }
    else
    {
        gtk_style_context_remove_class (style_context, "thumbnail");
    }
}

static void
update_captions (NautilusListViewItemUi *self)
{
    NautilusFile *file;
    g_autofree gchar *path_label = NULL;
    const gchar *snippet = NULL;

    file = nautilus_view_item_model_get_file (self->model);

    if (self->path_attribute_q != 0)
    {
        g_autofree gchar *path = NULL;
        g_autoptr (GFile) dir_location = NULL;
        g_autoptr (GFile) home_location = g_file_new_for_path (g_get_home_dir ());

        path = nautilus_file_get_string_attribute_q (file, self->path_attribute_q);
        dir_location = g_file_new_for_commandline_arg (path);

        if (g_file_equal (self->file_path_base_location, dir_location))
        {
            /* Only occurs when search result is
             * a direct child of the base location
             */
            path_label = NULL;
        }
        else if (g_file_has_prefix (dir_location, self->file_path_base_location))
        {
            g_autofree gchar *relative_path = NULL;

            relative_path = g_file_get_relative_path (self->file_path_base_location, dir_location);
            path_label = g_filename_display_name (relative_path);
        }
        else if (g_file_equal (dir_location, home_location))
        {
            path_label = nautilus_compute_title_for_location (home_location);
        }
        else if (self->file_path_base_location == NULL &&
                 g_file_has_prefix (dir_location, home_location))
        {
            g_autofree gchar *relative_path = NULL;

            relative_path = g_file_get_relative_path (home_location, dir_location);
            path_label = g_filename_display_name (relative_path);
        }
        else
        {
            path_label = g_file_get_path (dir_location);
        }
    }

    if (path_label != NULL)
    {
        gtk_label_set_text (GTK_LABEL (self->path), path_label);
        gtk_widget_show (self->path);
    }
    else
    {
        gtk_widget_hide (self->path);
    }

    if (self->show_snippet)
    {
        snippet = nautilus_file_get_search_fts_snippet (file);
    }
    if (snippet != NULL)
    {
        g_autoptr (GRegex) regex = NULL;
        g_autofree gchar *flattened_text = NULL;

        regex = g_regex_new ("\\R+", 0, G_REGEX_MATCH_NEWLINE_ANY, NULL);
        flattened_text = g_regex_replace (regex,
                                          snippet,
                                          -1,
                                          0,
                                          " ",
                                          G_REGEX_MATCH_NEWLINE_ANY,
                                          NULL);
        gtk_label_set_text (GTK_LABEL (self->snippet), flattened_text);
        gtk_widget_show (self->snippet);
    }
    else if (gtk_widget_is_visible (self->snippet))
    {
        gtk_widget_hide (self->snippet);
    }
}

static void
on_file_changed (NautilusListViewItemUi *self)
{
    NautilusFile *file;

    file = nautilus_view_item_model_get_file (self->model);

    update_icon (self);

    gtk_label_set_text (GTK_LABEL (self->label),
                        nautilus_file_get_display_name (file));
    update_captions (self);
}

static void
on_view_item_size_changed (GObject    *object,
                           GParamSpec *pspec,
                           gpointer    user_data)
{
    NautilusListViewItemUi *self = NAUTILUS_LIST_VIEW_ITEM_UI (user_data);

    update_icon (self);
}

static void
set_model (NautilusListViewItemUi *self,
           NautilusViewItemModel  *model);

static void
finalize (GObject *object)
{
    NautilusListViewItemUi *self = (NautilusListViewItemUi *) object;

    set_model (self, NULL);
    G_OBJECT_CLASS (nautilus_list_view_item_ui_parent_class)->finalize (object);
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    NautilusListViewItemUi *self = NAUTILUS_LIST_VIEW_ITEM_UI (object);

    switch (prop_id)
    {
        case PROP_MODEL:
        {
            g_value_set_object (value, self->model);
        }
        break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
on_view_item_is_cut_changed (GObject    *object,
                             GParamSpec *pspec,
                             gpointer    user_data)
{
    NautilusListViewItemUi *self = NAUTILUS_LIST_VIEW_ITEM_UI (user_data);
    gboolean is_cut;

    g_object_get (object, "is-cut", &is_cut, NULL);
    if (is_cut)
    {
        gtk_widget_add_css_class (self->icon, "cut");
    }
    else
    {
        gtk_widget_remove_css_class (self->icon, "cut");
    }
}

static void
set_model (NautilusListViewItemUi *self,
           NautilusViewItemModel  *model)
{
    NautilusFile *file;

    if (self->model == model)
    {
        return;
    }

    if (self->model != NULL)
    {
        g_signal_handlers_disconnect_by_data (self->model, self);
        g_clear_object (&self->model);
    }

    if (model == NULL)
    {
        return;
    }

    self->model = g_object_ref (model);

    file = nautilus_view_item_model_get_file (self->model);

    update_icon (self);
    gtk_label_set_text (GTK_LABEL (self->label),
                        nautilus_file_get_display_name (file));
    update_captions (self);

    g_signal_connect (self->model, "notify::icon-size",
                      (GCallback) on_view_item_size_changed, self);
    g_signal_connect (self->model, "notify::is-cut",
                      (GCallback) on_view_item_is_cut_changed, self);
    g_signal_connect_swapped (self->model, "file-changed",
                              (GCallback) on_file_changed, self);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    NautilusListViewItemUi *self = NAUTILUS_LIST_VIEW_ITEM_UI (object);

    switch (prop_id)
    {
        case PROP_MODEL:
        {
            set_model (self, g_value_get_object (value));
        }
        break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
nautilus_list_view_item_ui_class_init (NautilusListViewItemUiClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    object_class->finalize = finalize;
    object_class->get_property = get_property;
    object_class->set_property = set_property;

    g_object_class_install_property (object_class,
                                     PROP_MODEL,
                                     g_param_spec_object ("model",
                                                          "Item model",
                                                          "The item model that this UI reprensents",
                                                          NAUTILUS_TYPE_VIEW_ITEM_MODEL,
                                                          G_PARAM_READWRITE));

    gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/nautilus/ui/nautilus-list-view-item-ui.ui");

    gtk_widget_class_bind_template_child (widget_class, NautilusListViewItemUi, fixed_height_box);
    gtk_widget_class_bind_template_child (widget_class, NautilusListViewItemUi, icon);
    gtk_widget_class_bind_template_child (widget_class, NautilusListViewItemUi, label);
    gtk_widget_class_bind_template_child (widget_class, NautilusListViewItemUi, snippet);
    gtk_widget_class_bind_template_child (widget_class, NautilusListViewItemUi, path);
}

static void
nautilus_list_view_item_ui_init (NautilusListViewItemUi *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));

#if PANGO_VERSION_CHECK (1, 44, 4)
    {
        PangoAttrList *attr_list;

        /* GTK4 TODO: This attribute is set in the UI file but GTK 3 ignores it.
         * Remove this block after the switch to GTK 4. */
        attr_list = pango_attr_list_new ();
        pango_attr_list_insert (attr_list, pango_attr_insert_hyphens_new (FALSE));
        gtk_label_set_attributes (GTK_LABEL (self->label), attr_list);
        pango_attr_list_unref (attr_list);
    }
#endif
}

NautilusListViewItemUi *
nautilus_list_view_item_ui_new (void)
{
    return g_object_new (NAUTILUS_TYPE_LIST_VIEW_ITEM_UI, NULL);
}

void
nautilus_list_view_item_ui_set_model (NautilusListViewItemUi *self,
                                      NautilusViewItemModel  *model)
{
    g_object_set (self, "model", model, NULL);
}

NautilusViewItemModel *
nautilus_list_view_item_ui_get_model (NautilusListViewItemUi *self)
{
    NautilusViewItemModel *model = NULL;

    g_object_get (self, "model", &model, NULL);

    return model;
}

void
nautilus_list_view_item_ui_set_path (NautilusListViewItemUi *self,
                                     GQuark                  path_attribute_q,
                                     GFile                  *base_location)
{
    self->path_attribute_q = path_attribute_q;
    self->file_path_base_location = base_location;
}

void
nautilus_list_view_item_ui_show_snippet (NautilusListViewItemUi *self)
{
    self->show_snippet = TRUE;
}

gboolean
nautilus_list_view_item_ui_once (NautilusListViewItemUi *self)
{
    if (self->called_once)
    {
        return FALSE;
    }

    self->called_once = TRUE;
    return TRUE;
}
