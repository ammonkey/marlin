/*
 * Copyright (c) 2011 Lucas Baudin <xapantu@gmail.com>
 *
 * Marlin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Marlin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 */
using Marlin.View.Chrome;

public class Breadcrumbs : Marlin.View.Chrome.BasePathBar
{
    public Breadcrumbs(Gtk.UIManager ui) {}
    public override void load_right_click_menu(double x, double y) {}
    protected override void on_file_droped(List<GLib.File> uris, GLib.File target_file, Gdk.DragAction real_action) {}
}

void add_pathbar_tests()
{
/* TODO: they are broken currently */
    Test.add_func ("/marlin/pathbar/general", () => {
        Test.log_set_fatal_handler( () => { return false; });
        var breads = new Breadcrumbs(new Gtk.UIManager());
        var bread_entry = new BreadcrumbsEntry();
        assert(bread_entry is BreadcrumbsEntry);
        assert(bread_entry.text == "");
        Gdk.EventKey event = Gdk.EventKey();
        event.window = breads.get_window();
        event.keyval = 0x061; /* a */
        bread_entry.key_press_event(event);
        assert(bread_entry.text == "a");
        bread_entry.key_press_event(event);
        assert(bread_entry.text == "aa");
        assert(bread_entry.cursor == 2);

        event.keyval = 0xff51; /* left */
        bread_entry.key_press_event(event);
        assert(bread_entry.cursor == 1);
        bread_entry.key_press_event(event);
        assert(bread_entry.cursor == 0);
        bread_entry.key_press_event(event);
        assert(bread_entry.cursor == 0);
        event.keyval = 0xff53; /* right */
        bread_entry.key_press_event(event);
        assert(bread_entry.cursor == 1);

        /* insertion in the middle */
        event.keyval = 0x062; /* b */
        bread_entry.key_press_event(event);
        assert(bread_entry.text == "aba");
        event.keyval = 0x063; /* c */
        bread_entry.key_press_event(event);
        assert(bread_entry.text == "abca");

        /* insertion at the end */
        assert(bread_entry.cursor == 3);
        event.keyval = 0xff53; /* right */
        bread_entry.key_press_event(event);
        assert(bread_entry.cursor == 4);
        event.keyval = 0x064; /* d */
        bread_entry.key_press_event(event);
        assert(bread_entry.text == "abcad");
        
        /* backspace */
        event.keyval = 0xff08; /* backspace */
        bread_entry.key_press_event(event);
        assert(bread_entry.text == "abca");
        event.keyval = 0xff51; /* left */
        bread_entry.key_press_event(event);
        event.keyval = 0xff08; /* backspace */
        bread_entry.key_press_event(event);
        assert(bread_entry.text == "aba");
        event.keyval = 0xff53; /* right */
        bread_entry.key_press_event(event);
        event.keyval = 0x063; /* c */
        bread_entry.key_press_event(event);
        assert(bread_entry.text == "abac");
        event.keyval = 0xff51; /* left */
        bread_entry.key_press_event(event);
        
        /* selection */
        event.state = Gdk.ModifierType.SHIFT_MASK;
        event.keyval = 0xff51; /* left */
        bread_entry.key_press_event(event);
        assert(bread_entry.get_selection() == "a");
        bread_entry.key_press_event(event);
        assert(bread_entry.get_selection() == "ba");
        bread_entry.key_press_event(event);
        assert(bread_entry.get_selection() == "aba");
        bread_entry.key_press_event(event);
        assert(bread_entry.get_selection() == "aba");
        event.keyval = 0xff53; /* right */
        bread_entry.key_press_event(event);
        assert(bread_entry.get_selection() == "ba");
        bread_entry.key_press_event(event);
        assert(bread_entry.get_selection() == "a");
        bread_entry.key_press_event(event);
        assert(bread_entry.get_selection() == "");
        bread_entry.key_press_event(event);
        assert(bread_entry.get_selection() == "c");
        event.keyval = 0xff51; /* left */
        bread_entry.key_press_event(event);
        assert(bread_entry.get_selection() == "");
    });
    
    Test.add_func ("/marlin/pathbar/start-selection", () => {
        Test.log_set_fatal_handler( () => { return false; });
        var breads = new Breadcrumbs(new Gtk.UIManager());
        var bread_entry = new BreadcrumbsEntry();
        assert(bread_entry is BreadcrumbsEntry);
        assert(bread_entry.text == "");
        bread_entry.text = "abdcefghij/";
        bread_entry.cursor = ("abdcefghij/").length;
        Gdk.EventKey event = Gdk.EventKey();
        event.state = Gdk.ModifierType.SHIFT_MASK;
        event.window = breads.get_window();
        event.keyval = 0xff51; /* left */
        bread_entry.key_press_event(event);
        assert(bread_entry.get_selection() == "/");
    });
    
    Test.add_func ("/marlin/pathbar/backspace-without-text", () => {
        Test.log_set_fatal_handler( () => { return false; });
        var breads = new Breadcrumbs(new Gtk.UIManager());
        var bread_entry = new BreadcrumbsEntry();
        assert(bread_entry is BreadcrumbsEntry);
        assert(bread_entry.text == "");
        Gdk.EventKey event = Gdk.EventKey();
        event.window = breads.get_window();
        event.keyval = 0xff08; /* backspace */
        bread_entry.key_press_event(event);
        assert(bread_entry.get_selection() == null);
    });
    
    Test.add_func ("/marlin/pathbar/backspace-keypress", () => {
        Test.log_set_fatal_handler( () => { return false; });
        var breads = new Breadcrumbs(new Gtk.UIManager());
        BreadcrumbsEntry bread_entry = breads.entry;
        assert(bread_entry is BreadcrumbsEntry);
        assert(bread_entry.text == "");
        Gdk.EventKey event = Gdk.EventKey();
        event.window = breads.get_window();
        event.keyval = 0xff08; /* backspace */
        breads.key_press_event(event);
        assert(bread_entry.get_selection() == null);

        event.keyval = 0x061; /* a */
        breads.change_breadcrumbs("");
        breads.change_breadcrumbs("/");
    });
    
    Test.add_func ("/marlin/pathbar/go-to-trash", () => {
        Test.log_set_fatal_handler( () => { return false; });
        var breads = new Breadcrumbs(new Gtk.UIManager());
        breads.change_breadcrumbs("trash:///");
        breads.change_breadcrumbs("/home/there");
    });
    
    Test.add_func ("/marlin/pathbar/non-file-system-location", () => {
        Test.log_set_fatal_handler( () => { return false; });
        var breads = new Breadcrumbs(new Gtk.UIManager());
        breads.change_breadcrumbs("trash://");
        breads.change_breadcrumbs("trash://dir/folder/");
        assert(breads.get_elements_path() == "trash://dir/folder/");
        breads.change_breadcrumbs("ftp://test@test.com/dir/folder/");
        assert(breads.get_elements_path() == "ftp://test@test.com/dir/folder/");
    });

}
 
void main(string[] args)
{
    Gtk.init(ref args);
    Test.init(ref args);
    //Preferences.settings = new GLib.Settings("org.gnome.marlin.preferences");

    add_pathbar_tests ();

    Idle.add (() => {
        Test.run ();
        Gtk.main_quit ();
        return false;
        }
    );
    Gtk.main ();
}
