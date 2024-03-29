/*
 * Copyright (C) 2011 Marlin Developers
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Authors :   Lucas Baudin <xapantu@gmail.com>
 *              ammonkey <am.monkeyd@gmail.com>
 *
 */

using Varka.Widgets;

namespace Marlin.View
{
    public class SettingsDialog : Gtk.Dialog
    {
        public SettingsDialog(Window win)
        {
            set_title(_("Marlin Preferences"));
            set_resizable(false);
            set_size_request (360, -1);
            //border_width = 5;

            var mai_notebook = new Varka.Widgets.StaticNotebook();

            var first_vbox = new Gtk.Box(Gtk.Orientation.VERTICAL, 3);

            /* Single click */
            var checkbox = new Gtk.Switch();

            Preferences.settings.bind("single-click", checkbox , "active", SettingsBindFlags.DEFAULT);

            var hbox_single_click = new Gtk.Box(Gtk.Orientation.HORIZONTAL, 0);
            var label = new Gtk.Label(_("Single click to open:"));
            label.set_alignment(0, 0.5f);
            hbox_single_click.pack_start(label);
            hbox_single_click.pack_start(checkbox, false, false);
            first_vbox.pack_start(hbox_single_click, false);

            /* Mouse selection speed */
            var spi_click_speed = new Gtk.Scale.with_range(Gtk.Orientation.HORIZONTAL, 0, 1000, 1);

            hbox_single_click = new Gtk.Box(Gtk.Orientation.HORIZONTAL, 0);
            label = new Gtk.Label(_("Mouse auto-selection speed:"));
            label.set_alignment(0, 0.5f);
            hbox_single_click.pack_start(label);
            hbox_single_click.pack_start(spi_click_speed, true, true);

            Preferences.settings.bind("single-click", hbox_single_click, "sensitive", SettingsBindFlags.DEFAULT);

            Preferences.settings.bind("single-click-timeout", spi_click_speed.get_adjustment(), "value", SettingsBindFlags.DEFAULT);
            
            first_vbox.pack_start(hbox_single_click, false);

            /* Make default FM */
            var sw_fm_default = new Gtk.Switch ();
            sw_fm_default.active = is_marlin_mydefault_fm ();
            
            var hbox_fm_default = new Gtk.Box(Gtk.Orientation.HORIZONTAL, 0);
            var l2 = new Gtk.Label(_("Default File Manager:"));
            l2.set_alignment(0, 0.5f);
            hbox_fm_default.pack_start(l2);
            hbox_fm_default.pack_start(sw_fm_default, false, false);
            first_vbox.pack_start(hbox_fm_default, false);
            sw_fm_default.notify["active"].connect((s, p) => {
                                                   make_marlin_default_fm (sw_fm_default.get_active ());
                                                   });
            
            mai_notebook.append_page(first_vbox, new Gtk.Label(_("Behavior")));

            first_vbox = new Gtk.Box(Gtk.Orientation.VERTICAL, 3);
            
            /* Date format */
            var mode_date_format = new ModeButton();
            mode_date_format.append(new Gtk.Label(_("iso")));
            mode_date_format.append(new Gtk.Label(_("locale")));
            mode_date_format.append(new Gtk.Label(_("informal")));
            mode_date_format.selected = (int)Preferences.settings.get_enum ("date-format");

            mode_date_format.mode_changed.connect(date_format_changed);

            hbox_single_click = new Gtk.Box(Gtk.Orientation.HORIZONTAL, 0);

            label = new Gtk.Label(_("Date format:"));
            label.set_alignment(0, 0.5f);

            hbox_single_click.pack_start(label);
            hbox_single_click.pack_start(mode_date_format, false, false);
            
            first_vbox.pack_start(hbox_single_click, false);

            mai_notebook.append_page(first_vbox, new Gtk.Label(_("Display")));

            first_vbox = new Gtk.Box(Gtk.Orientation.VERTICAL, 3);
            
            var view = new Gtk.TreeView();
            
            Gdk.RGBA color = Gdk.RGBA();
            first_vbox.get_style_context ().get_background_color (Gtk.StateFlags.NORMAL);
            view.override_background_color (Gtk.StateFlags.NORMAL, color);

            var listmodel = new Gtk.ListStore (2, typeof (string), typeof (bool));
            view.set_model (listmodel);
            view.set_headers_visible (false);
            var column = new Gtk.TreeViewColumn();

            var text_renderer = new Gtk.CellRendererText();
            column.pack_start(text_renderer, true);
            column.set_attributes(text_renderer, "text", 0);
            var toggle = new Gtk.CellRendererToggle();
            toggle.toggled.connect_after ((toggle, path) => 
            {
                var tree_path = new Gtk.TreePath.from_string (path);
                Gtk.TreeIter iter;
                listmodel.get_iter (out iter, tree_path);
                var name = Value(typeof(string));
                var active = Value(typeof(bool));
                listmodel.get_value(iter, 0, out name);
                listmodel.get_value(iter, 1, out active);
                listmodel.set (iter, 1, !active.get_boolean());
                if(active.get_boolean() == false)
                {
                    enable_plugin(name.get_string());
                }
                else
                {
                    disable_plugin(name.get_string());
                }
            });
            column.pack_start(toggle, false);
            column.set_attributes(toggle, "active", 1);
            
            view.insert_column(column, -1);

            Gtk.TreeIter iter;

            foreach(var plugin_name in (plugins.get_available_plugins()))
            {
                listmodel.append (out iter);
                listmodel.set (iter, 0, plugin_name, 1, plugin_name in Preferences.settings.get_strv("plugins-enabled"));
            }

            first_vbox.pack_start(view);

            mai_notebook.append_page(first_vbox, new Gtk.Label(_("Extensions")));

            /* Margins
             * we don't set the margin_top to get the staticnotebook the most upper possible
             * otherwise we would have used the border-width property of the dialog */
            get_content_area ().margin_left = 5;
            get_content_area ().margin_right = 5;
            get_content_area ().margin_bottom = 5;
            mai_notebook.margin_left = 5;
            mai_notebook.margin_right = 5;
            mai_notebook.margin_bottom = 12;
            
            ((Gtk.Box)get_content_area()).pack_start(mai_notebook);

            this.show_all();

            add_buttons("gtk-close", Gtk.ResponseType.CLOSE);
        }
        
        void disable_plugin(string name)
        {
            
            if(!plugins.disable_plugin(name))
            {
                critical("Can't properly disable the plugin %s!", name);
            }
        }
        
        void enable_plugin(string name)
        {
            string[] plugs = new string[Preferences.settings.get_strv("plugins-enabled").length + 1];
            string[] current_plugins = Preferences.settings.get_strv("plugins-enabled");

            for(int i = 0; i < current_plugins.length; i++)
            {
                plugs[i] = current_plugins[i];
            }
            plugs[plugs.length - 1] = name;
            Preferences.settings.set_strv("plugins-enabled", plugs);

            plugins.load_plugin(name);
        }

        private void date_format_changed(Gtk.Widget widget)
        {
            int value = 0; /* iso */
            switch(((Gtk.Label)widget).get_text())
            {
            case "iso":
                value = 0;
                break;
            case "locale":
                value = 1;
                break;
            case "informal":
                value = 2;
                break;
            }
            Preferences.settings.set_enum ("date-format", value);
        }

        public override void response(int id)
        {
            switch(id)
            {
            case Gtk.ResponseType.CLOSE:
                destroy();
                break;
            }
        }
            
        private bool is_marlin_mydefault_fm ()
        {
            bool trash_uri_is_default = false;
            bool foldertype_is_default = "marlin.desktop" == AppInfo.get_default_for_type("inode/directory", false).get_id();
            AppInfo? app_trash_handler = AppInfo.get_default_for_type("x-scheme-handler/trash", true);
            if (app_trash_handler != null)
                trash_uri_is_default = "marlin.desktop" == app_trash_handler.get_id();

            return foldertype_is_default && trash_uri_is_default;
        }

        private void make_marlin_default_fm (bool active)
        {
            if (active) {
                AppInfo marlin_app = (AppInfo) new DesktopAppInfo ("marlin.desktop");
                if (marlin_app != null) {
                    try {
                        marlin_app.set_as_default_for_type ("inode/directory");
                        marlin_app.set_as_default_for_type ("x-scheme-handler/trash");
                    } catch (GLib.Error e) {
                        critical ("Can't set Marlin default FM: %s", e.message);
                    }
                }
            } else {
                AppInfo.reset_type_associations ("inode/directory");
                AppInfo.reset_type_associations ("x-scheme-handler/trash");            
            }
        }
    }
}
