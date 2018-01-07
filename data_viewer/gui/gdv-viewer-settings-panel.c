/*
 * gdv-viewer-settings-panel.c
 * This file is part of gdv
 *
 * Copyright (C) 2013 - Emanuel Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>

#include "gui/gdv-viewer-settings-panel.h"
#include "file/viewer-file.h"

//struct _ViewerSourceView
//{
//  GtkSourceView parent;
//};

/* Define Properties */
enum
{
  PROP_0,

  PROP_CURRENT_FILE,
  PROP_ROW_SEPARATOR,
  PROP_COL_SEPARATOR,

  N_PROPERTIES
};

struct _GdvViewerSettingsPanelClass
{
  GtkBoxClass parent;
};

typedef struct _GdvViewerSettingsPanelPrivate GdvViewerSettingsPanelPrivate;

struct _GdvViewerSettingsPanelPrivate
{
  GtkWidget *xmpl;
  guint pressed_entry_icon_to;

  ViewerFile *current_file;
  GtkFrame   *file_frame;

  gchar *row_separator;
  gchar *col_separator;
};

static GParamSpec *properties [N_PROPERTIES];

G_DEFINE_TYPE_WITH_PRIVATE (GdvViewerSettingsPanel, gdv_viewer_settings_panel, GTK_TYPE_BOX);

static void
on_widespace_changed (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       app);

static void
on_tabulator_changed (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       app);

static void
on_line_feed_changed (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       app);

static GActionEntry separator_actions[] =
{
  { "widespace-changed", on_widespace_changed, NULL, NULL, NULL },
  { "tabulator-changed", on_tabulator_changed, NULL, NULL, NULL },
  { "line-feed-changed", on_line_feed_changed, NULL, NULL, NULL },
};


static void
gdv_viewer_settings_panel_init (GdvViewerSettingsPanel *self)
{
  GdvViewerSettingsPanelPrivate *priv;
//  GtkWidget *tmp;

  gtk_widget_init_template (GTK_WIDGET (self));
  priv = gdv_viewer_settings_panel_get_instance_private (self);
  priv->current_file = NULL;
  priv->pressed_entry_icon_to = 0;

  priv->row_separator = g_strdup("[\n]");
  priv->col_separator = g_strdup("[ ]");

/*  const gchar **list = gtk_widget_list_action_prefixes (GTK_WIDGET (self));

  g_print ("REACHED\n");

  while (list)
  {
    const gchar *curr_list = *list;

    g_print ("Prefix: %s\n", curr_list);
    list = list+1;
  }
*/
//  g_action_map_add_action_entries (G_ACTION_MAP (self),
//                                   app_entries, G_N_ELEMENTS (app_entries),
//                                   application);


/*

  tmp = g_object_new (GTK_TYPE_IMAGE,
                      "icon-name", "panel-left-pane-symbolic",
                      "margin-start", 12,
                      "margin-end", 12,
                      "visible", TRUE,
                      NULL);

  gtk_container_add (self, tmp);
*/
  /*
//  gtk_widget_init_template (GTK_WIDGET (view));
//  GtkTextBuffer *new_buffer = gtk_text_buffer_new (NULL);
  GtkSourceBuffer *new_buffer = gtk_source_buffer_new (NULL);
//  GtkTextTag *new_tag = gtk_text_tag_new ("New Tag");
  GtkTextIter start, end;
//  GdkRGBA rgba;
  GtkTextTag *tag;
*/

/*

  gtk_text_buffer_set_text (GTK_TEXT_BUFFER (new_buffer),
    "Hello my world\n"
    "Hello my other world\tsafrasfa\tfsafsa\n", -1);

  tag = gtk_text_buffer_create_tag (GTK_TEXT_BUFFER (new_buffer),
                                    "blue_foreground",
                                    "foreground", "blue", NULL);
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER (new_buffer), &start, 7);
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER (new_buffer), &end, 12);
  gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER (new_buffer), tag, &start, &end);
  tag = gtk_text_buffer_create_tag (GTK_TEXT_BUFFER (new_buffer),
                                    "blackbackground",
                                    "background", "black", NULL);
  gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER (new_buffer), tag, &start, &end);

  gtk_text_view_set_buffer (GTK_TEXT_VIEW (view), GTK_TEXT_BUFFER (new_buffer));
*/
}

static void
gdv_viewer_settings_panel_get_property (GObject    *object,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  GdvViewerSettingsPanel *self = (GdvViewerSettingsPanel *)object;
  GdvViewerSettingsPanelPrivate *priv;

  priv = gdv_viewer_settings_panel_get_instance_private (self);

  switch (prop_id)
  {
    case PROP_CURRENT_FILE:
      g_value_set_object (value, priv->current_file);
      break;
    case PROP_ROW_SEPARATOR:
      g_value_set_string (value, priv->row_separator);
      break;
    case PROP_COL_SEPARATOR:
      g_value_set_string (value, priv->col_separator);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
_set_file (GdvViewerSettingsPanel *self,
           ViewerFile             *file)
{
  GdvViewerSettingsPanelPrivate *priv;

  priv = gdv_viewer_settings_panel_get_instance_private (self);

  priv->current_file = file;

  if (file)
  {
    gchar *file_name;
    GFile *curr_g_file;

    g_object_get (file, "file", &curr_g_file, NULL);
    if (curr_g_file)
    {
      file_name = g_file_get_basename (curr_g_file);

      gtk_frame_set_label (priv->file_frame, file_name);
      g_object_set (G_OBJECT (priv->file_frame), "visible", TRUE, NULL);
      return;
    }
  }

  g_object_set (G_OBJECT (priv->file_frame), "visible", FALSE, NULL);
}

static void
gdv_viewer_settings_panel_set_property (GObject      *object,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  GdvViewerSettingsPanel *self = (GdvViewerSettingsPanel *)object;
  GdvViewerSettingsPanelPrivate *priv;

  priv = gdv_viewer_settings_panel_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_CURRENT_FILE:
      _set_file(self, g_value_get_object (value));
      break;
    case PROP_ROW_SEPARATOR:
      if (priv->row_separator != NULL)
        g_free (priv->row_separator);

      priv->row_separator = g_value_dup_string (value);
      break;
    case PROP_COL_SEPARATOR:
      if (priv->col_separator != NULL)
        g_free (priv->col_separator);

      priv->col_separator = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
on_row_settings_pan_activate (GtkEntry *entry,
               gpointer  user_data)
{
  g_print("REACHED ROW ACTIVATE\n");



  if (user_data)
    g_print ("\tUser_data: %s\n", g_type_name (G_OBJECT_TYPE (user_data)));
  g_print ("\tENTRY SET TO: %s\n", gtk_entry_get_text(entry));
}

static void
on_col_settings_pan_activate (GtkEntry *entry,
               gpointer  user_data)
{
  g_print("REACHED COL ACTIVATE\n");



  if (user_data)
    g_print ("\tUser_data: %s\n", g_type_name (G_OBJECT_TYPE (user_data)));
  g_print ("\tENTRY SET TO: %s\n", gtk_entry_get_text(entry));
}

static gboolean
hold_entry_symbol_cb (gpointer user_data)
{
  GdvViewerSettingsPanelPrivate *priv;
  GdvViewerSettingsPanel *self = GDV_VIEWER_SETTINGS_PANEL (user_data);

  priv = gdv_viewer_settings_panel_get_instance_private (self);

  /* FIXME: open menu and register callback to release-button */

  if (priv->pressed_entry_icon_to)
    priv->pressed_entry_icon_to = 0;

  return FALSE;
}

static void
on_settings_pan_icon_press (GtkEntry *entry,
               GtkEntryIconPosition icon_pos,
               GdkEvent            *event,
               gpointer             user_data)
{
  GdvViewerSettingsPanelPrivate *priv;
  GdvViewerSettingsPanel *self = GDV_VIEWER_SETTINGS_PANEL (user_data);

  g_print("REACHED ICON-PRESS\n");

  priv = gdv_viewer_settings_panel_get_instance_private (self);

  priv->pressed_entry_icon_to =
    g_timeout_add (300, hold_entry_symbol_cb , self);

  if (user_data)
    g_print ("\tUser_data: %s\n", g_type_name (G_OBJECT_TYPE (user_data)));
}


static void
on_settings_pan_icon_release (GtkEntry *entry,
               GtkEntryIconPosition icon_pos,
               GdkEvent            *event,
               gpointer             user_data)
{
  GdvViewerSettingsPanelPrivate *priv;
  GdvViewerSettingsPanel *self = GDV_VIEWER_SETTINGS_PANEL (user_data);

  g_print("REACHED ICON-RELEASE\n");

  priv = gdv_viewer_settings_panel_get_instance_private (self);

  if (priv->pressed_entry_icon_to)
  {
    GtkBuilder *builder;
    GMenuModel *app_menu;
    GtkWidget *menu;
//    GActionGroup *new_map = g_object_new (g_action_group_get_type(), NULL);
    GSimpleActionGroup *group;

    g_source_remove (priv->pressed_entry_icon_to);
    priv->pressed_entry_icon_to = 0;

    builder =
      gtk_builder_new_from_resource ("/net/gdv/viewerapp/ui/gui/gdv-viewer-settings-entry-menu.ui");
    app_menu = G_MENU_MODEL (gtk_builder_get_object (builder, "entry-menu"));
    menu = gtk_menu_new_from_model (app_menu);

    group = g_simple_action_group_new ();

    gtk_widget_insert_action_group(GTK_WIDGET (menu), "entry", G_ACTION_GROUP(group));

    g_action_map_add_action_entries (G_ACTION_MAP (group),
                                     separator_actions, G_N_ELEMENTS (separator_actions),
                                     entry);

//  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
//                                           on_settings_pan_icon_press);
//  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
//                                           on_settings_pan_icon_release);

//    g_action_map_add_action_entries (G_ACTION_MAP (app_menu),
//                                     separator_actions, G_N_ELEMENTS (separator_actions),
//                                     entry);

//    menu = gtk_popover_new_from_model(GTK_WIDGET (entry), app_menu);

    gtk_menu_popup_at_pointer (GTK_MENU(menu), event);
//    gtk_popover_popup (GTK_POPOVER(menu));

    g_object_unref (builder);
  }

  if (user_data)
    g_print ("\tUser_data: %s\n", g_type_name (G_OBJECT_TYPE (user_data)));
}

static void
gdv_viewer_settings_panel_finalize (GObject *object)
{
  GdvViewerSettingsPanel *self = (GdvViewerSettingsPanel *)object;
  GdvViewerSettingsPanelPrivate *priv;

  priv = gdv_viewer_settings_panel_get_instance_private (self);

  if (priv->col_separator)
    g_free (priv->col_separator);
  if (priv->row_separator)
    g_free (priv->row_separator);

  G_OBJECT_CLASS (gdv_viewer_settings_panel_parent_class)->finalize (object);
}

static void
gdv_viewer_settings_panel_class_init (GdvViewerSettingsPanelClass *klass)
{
  GObjectClass *object_class;

  object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gdv_viewer_settings_panel_finalize;

  gtk_widget_class_set_template_from_resource (
    GTK_WIDGET_CLASS (klass),
    "/net/gdv/viewerapp/ui/gui/gdv-viewer-settings-panel.ui");

  object_class->get_property = gdv_viewer_settings_panel_get_property;
  object_class->set_property = gdv_viewer_settings_panel_set_property;

//  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
//                                               "/org/gtk/exampleapp/window.ui");
//  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ExampleAppWindow, stack);

  properties [PROP_CURRENT_FILE] =
    g_param_spec_object ("file",
                         "File",
                         "The current file",
                         VIEWER_TYPE_FILE,
                         G_PARAM_READWRITE);

  properties [PROP_ROW_SEPARATOR] =
    g_param_spec_string ("row-separator",
                         "the row-separator",
                         "The string to separate rows",
                         "[\n]",
                         G_PARAM_READWRITE);

  properties [PROP_COL_SEPARATOR] =
    g_param_spec_string ("column-separator",
                         "the column-separator",
                         "The string to separate column",
                         "[ ]",
                         G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
                                           on_row_settings_pan_activate);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
                                           on_col_settings_pan_activate);

  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
                                           on_settings_pan_icon_press);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
                                           on_settings_pan_icon_release);

//  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
//                                           on_tabulator_changed);
//  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (object_class),
//                                           on_widespace_changed);

  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (object_class),
                                                GdvViewerSettingsPanel,
                                                file_frame);
}

GdvViewerSettingsPanel *gdv_viewer_settings_panel_new (void)
{
  return g_object_new (gdv_viewer_settings_panel_get_type(), NULL);
}


static void
on_widespace_changed (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       app)
{
  gtk_entry_set_text (GTK_ENTRY(app), "[ ]");
  g_signal_emit_by_name (GTK_ENTRY(app), "activate", NULL);
}

static void
on_tabulator_changed (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       app)
{
  gtk_entry_set_text (GTK_ENTRY(app), "[\\t]");
  g_signal_emit_by_name (GTK_ENTRY(app), "activate", NULL);
}

static void
on_line_feed_changed (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       app)
{
  gtk_entry_set_text (GTK_ENTRY(app), "[\\n]");
  g_signal_emit_by_name (GTK_ENTRY(app), "activate", NULL);
}

