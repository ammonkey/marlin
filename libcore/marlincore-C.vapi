using Gtk;
using GLib;

[CCode (cprefix = "", lower_case_cprefix = "", cheader_filename = "config.h")]
namespace Config {
    //public const string GETTEXT_PACKAGE;
    public const string PIXMAP_DIR;
    public const string UI_DIR;
    public const string VERSION;
    /*public const string PACKAGE_NAME;
      public const string PACKAGE_VERSION;
      public const string VERSION;*/
}

[CCode (cprefix = "FM", lower_case_cprefix = "fm_", cheader_filename = "fm-list-model.h")]
namespace FM
{
    public class ListModel : Object, Gtk.TreeModel, Gtk.TreeDragDest, Gtk.TreeSortable
    {
        public bool load_subdirectory(Gtk.TreePath path, out GOF.Directory.Async dir);
        public void add_file(GOF.File file, GOF.Directory.Async dir);
        public GOF.File file_for_path(Gtk.TreePath path);
    }
}

[CCode (cprefix = "", lower_case_cprefix = "", cheader_filename = "marlin-global-preferences.h")]
namespace Preferences {
    public GLib.Settings settings;
    public GLib.Settings marlin_icon_view_settings;
    public string tags_colors[10];
}

[CCode (cprefix = "MarlinFileOperations", lower_case_cprefix = "marlin_file_operations_", cheader_filename = "marlin-file-operations.h")]
namespace Marlin.FileOperations {
    static void empty_trash(Gtk.Widget widget);
}

/*[CCode (cprefix = "MarlinTrashMonitor", lower_case_cprefix = "marlin_trash_monitor_", cheader_filename = "marlin-trash-monitor.h")]
namespace Marlin.TrashMonitor {
    static bool is_empty ();
}*/


public static uint action_new (GLib.Type type, string signal_name);

[CCode (cprefix = "EelGtk", lower_case_cprefix = "eel_gtk_window_", cheader_filename = "eel-gtk-extensions.h")]
namespace EelGtk.Window {
    public string get_geometry_string (Gtk.Window win);
    public void set_initial_geometry_from_string (Gtk.Window win, string geometry, uint w, uint h, bool ignore_position);
}
[CCode (cprefix = "Eel", lower_case_cprefix = "eel_", cheader_filename = "eel-gtk-extensions.h")]
namespace Eel {
    public void pop_up_context_menu (Gtk.Menu menu, int16 offset_x, int16 offset_y, Gdk.EventButton event);
}

[CCode (cprefix = "Eel", lower_case_cprefix = "eel_", cheader_filename = "eel-fcts.h")]
namespace Eel {
    public string? get_date_as_string (uint64 d, string format);
    public GLib.List? get_user_names ();
    public bool get_user_id_from_user_name (string *user_name, out int uid);
    public bool get_group_id_from_group_name (string *group_name, out int gid);
    public bool get_id_from_digit_string (string digit_str, out int id);
    public string format_size (uint64 size);
}

[CCode (cprefix = "EelPango", lower_case_cprefix = "eel_pango_", cheader_filename = "eel-pango-extensions.h")]
namespace EelPango {
    public unowned Pango.AttrList attr_list_small();
    public unowned Pango.AttrList attr_list_big();
}

[CCode (cprefix = "Nautilus", lower_case_cprefix = "nautilus_")]
namespace Nautilus {
    [CCode (cheader_filename = "nautilus-icon-info.h")]
    public class IconInfo : GLib.Object{
        public static IconInfo lookup(GLib.Icon icon, int size);
        public Gdk.Pixbuf get_pixbuf_nodefault();
        public Gdk.Pixbuf get_pixbuf_at_size(int size);
    }
}

[CCode (cprefix = "Marlin", lower_case_cprefix = "marlin_")]
namespace Marlin
{
    [CCode (cheader_filename = "marlin-abstract-sidebar.h")]
    public abstract class AbstractSidebar : Gtk.ScrolledWindow
    {
        public void add_extra_item(string text);
    }
    [CCode (cheader_filename = "marlin-trash-monitor.h")]
    public abstract class TrashMonitor : Object
    {
        public static TrashMonitor get();
        public static bool is_empty ();

        public signal void trash_state_changed (bool new_state);
    }
}

[CCode (cprefix = "GOF", lower_case_cprefix = "gof_")]
namespace GOF {

    [CCode (cheader_filename = "gof-file.h")]
    public class File : GLib.Object {
        public File(GLib.File location, GLib.File dir);
        public static File get(GLib.File location);
        public static File cache_lookup (GLib.File file);
        public void remove_from_caches ();
        public GLib.File location;
        public GLib.File directory; /* parent directory location */
        public GLib.Icon? icon;
        public GLib.FileInfo? info;
        public unowned string name;
        public string basename;
        public string uri;
        public uint64 size;
        public string format_size;
        public string color;
        public string formated_modified;
        public string formated_type;
        public unowned string ftype;
        public Gdk.Pixbuf pix;
        public unowned string trash_orig_path;

        public FileType file_type;
        public bool is_hidden;
        public bool is_directory;
        public bool is_symlink();
        public bool is_trashed();
        public bool link_known_target;
        public unowned string thumbnail_path;
        public uint flags;
        public string get_formated_time (string attr);
        public Nautilus.IconInfo get_icon (int size, FileIconFlags flags);
        public Gdk.Pixbuf get_icon_pixbuf (int size, bool forced_size, FileIconFlags flags);

        public bool is_mounted;
        public bool exists;

        public int uid;
        public int gid;
        public string owner;
        public string group;
        public bool has_permissions;
        public uint32 permissions;

        public void update ();
        public void update_icon (int size);
        public bool can_set_owner ();
        public bool can_set_group ();
        public bool can_set_permissions ();
        public string get_permissions_as_string ();
        public bool launch_with(Gdk.Screen screen, AppInfo app);

        public GLib.List? get_settable_group_names ();
            
        public signal void info_available ();
    }

    [CCode (cheader_filename = "gof-file.h")]
    public enum FileIconFlags
    {
        NONE,
        USE_THUMBNAILS
    }
}

[CCode (cprefix = "GOF", lower_case_cprefix = "gof_")]
namespace GOF {
    [CCode (cheader_filename = "gof-abstract-slot.h")]
    public class AbstractSlot : GLib.Object {
        public void add_extra_widget(Gtk.Widget widget);
    }
}
