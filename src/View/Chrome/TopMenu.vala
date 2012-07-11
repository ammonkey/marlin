//
//  TopMenu.cs
//
//  Authors:
//       mathijshenquet <mathijs.henquet@gmail.com>
//       ammonkey <am.monkeyd@gmail.com>
//
//  Copyright (c) 2010 mathijshenquet
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
using Gtk;

namespace Marlin.View.Chrome
{
    public class TopMenu : Gtk.Toolbar
    {
        public ViewSwitcher? view_switcher;
        public Gtk.Menu compact_menu;
        public Gtk.Menu toolbar_menu;
        public Varka.Widgets.ToolButtonWithMenu app_menu;
        public LocationBar? location_bar;
        public Window win;

        public TopMenu (Window window)
        {
            win = window;
            /*if (Preferences.settings.get_boolean("toolbar-primary-css-style"))
	            get_style_context().add_class ("primary-toolbar");*/
            get_style_context().add_class ("menubar");
            set_icon_size (Gtk.IconSize.MENU);

            compact_menu = (Gtk.Menu) win.ui.get_widget("/CompactMenu");
            toolbar_menu = (Gtk.Menu) win.ui.get_widget("/ToolbarMenu");

            app_menu = new Varka.Widgets.ToolButtonWithMenu (new Image.from_icon_name ("emblem-system-symbolic", IconSize.MENU), 
                                                             null, _("Menu"), compact_menu);
            setup_items();
            show();
            
            button_press_event.connect(right_click);
        }
        
        public bool right_click (Gdk.EventButton event)
        {
            if(event.button == 3)
            {
                right_click_extern(event);
                return true;
            }
            return false;
        }
        
        public void right_click_extern (Gdk.EventButton event)
        {
            Eel.pop_up_context_menu(toolbar_menu, 0, 0, event);
        }

        public void setup_items ()
        {
            if (compact_menu != null)
                compact_menu.ref();
            @foreach (toolitems_destroy);
            string[]? toolbar_items = Preferences.settings.get_strv("toolbar-items");
            string name;
            bool linked_nav = false;
            //foreach (string name in toolbar_items) {
            for (var i=0; (name = toolbar_items[i]) != null; i++) {
                name = toolbar_items[i];
                if (name == "Separator")
                {
                    Gtk.SeparatorToolItem? sep = new Gtk.SeparatorToolItem ();
                    sep.set_draw(true);
                    sep.show();
                    insert(sep, -1);
                    continue;
                }
                if (name == "LocationEntry")
                {
                    location_bar = new LocationBar (win.ui, win);

                    /* init the path if we got a curent tab with a valid slot
                       and a valid directory loaded */
                    if (win.current_tab != null && win.current_tab.slot != null
                        && win.current_tab.slot.directory != null) {
                        location_bar.path = win.current_tab.slot.directory.location.get_parse_name ();
                        //debug ("topmenu test path %s", location_bar.path);
                    }

                    location_bar.escape.connect( () => { ((FM.Directory.View) win.current_tab.slot.view_box).grab_focus(); });
                    location_bar.activate.connect(() => { win.current_tab.path_changed(File.new_for_commandline_arg(location_bar.path)); });
                    location_bar.activate_alternate.connect((a) => { win.add_tab(File.new_for_commandline_arg(a)); });
                    location_bar.show_all();
                    location_bar.margin_right = 2;
                    location_bar.margin_left = 2;
                    insert(location_bar, -1);
                    continue;
                }
                if (name == "ViewSwitcher")
                {
                    view_switcher = new ViewSwitcher (win.main_actions);
                    view_switcher.show_all();
                    view_switcher.margin_right = 2;
                    view_switcher.margin_left = 2;
                    insert(view_switcher, -1);
                    continue;
                }

                Gtk.ToolItem? item;
                Gtk.Action? main_action = win.main_actions.get_action(name);

                if (main_action != null)
                {
                    if ( name == "Back"){
                        var next_item = toolbar_items[i+1];
                        if (next_item != null && next_item == "Forward") {
                            linked_nav = true;
                            item = create_nav_buttons ();
                            item.margin_right = 2;
                            if (i != 0)
                                item.margin_left = 2;
                        } else {
                            win.button_back = new Varka.Widgets.ToolButtonWithMenu.from_action (main_action);
                            win.button_back.show_all();
                            item = win.button_back;
                        }
                    } else if (name == "Forward"){
                        if (linked_nav)
                            continue;
                        win.button_forward = new Varka.Widgets.ToolButtonWithMenu.from_action (main_action);
                        win.button_forward.show_all();
                        item = win.button_forward;
                    } else {
                        item = (ToolItem) main_action.create_tool_item();
                        item.get_child ().button_press_event.connect (right_click);
                        item.margin_right = 1;
                        item.margin_left = 1;
                    }
                    insert(item, -1);
                }
            }

            app_menu.margin_left = 2;
            insert(app_menu, -1);
        }

        private void toolitems_destroy (Gtk.Widget? w) {
            ((Gtk.Container)this).remove (w);
        }

        private Box navbox;

        private ToolItem create_nav_buttons () {
            Gtk.Action? main_action;
            ToolItem item = new ToolItem ();

            navbox = new Box (Gtk.Orientation.HORIZONTAL, 0);
            main_action = win.main_actions.get_action("Back");
            win.navi_btn_back = new Varka.Widgets.ButtonWithMenu.from_action(main_action);
            navbox.add (win.navi_btn_back);
            main_action = win.main_actions.get_action("Forward");
            win.navi_btn_forward = new Varka.Widgets.ButtonWithMenu.from_action(main_action);
            navbox.add (win.navi_btn_forward);
            item.add (navbox);
            item.show_all();

            item.toolbar_reconfigured.connect (() => { 
                ReliefStyle rs = get_relief_style ();
                win.navi_btn_back.relief = rs;
                win.navi_btn_forward.relief = rs;

                navbox.get_style_context ().remove_class ("linked");
                if (rs == ReliefStyle.NORMAL)
                navbox.get_style_context ().add_class ("linked");
            });

            return item;
        }
    }
}

