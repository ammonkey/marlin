/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2010 Jordi Puigdellívol <jordi@gloobus.net>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * version 3.0 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3.0 for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authors: Jordi Puigdellívol <jordi@gloobus.net>
 *          ammonkey <am.monkeyd@gmail.com>
 *

 Marlin Tagging system

//Dependences
libsqlite3-dev

//To create de table
create table tags (uri TEXT, color INT, tags TEXT);

//To compile this sample
valac --pkg sqlite3 -o sqlitesample SqliteSample.vala
valac --pkg sqlite3 --pkg gio-2.0 -o sqlitesample marlin_tagging.vala && ./sqlitesample

*/

using GLib;
using Sqlite;

[DBus (name = "org.elementary.marlin.db")]
public class MarlinTags : Object {

    protected static Sqlite.Database db;

    public MarlinTags(){
        openMarlinDB();
    }

    protected static void fatal(string op, int res) {
        error("%s: [%d] %s", op, res, db.errmsg());
    }

    private static int show_table_callback (int n_columns, string[] values, string[] column_names)
    {
        for (int i = 0; i < n_columns; i++) {
            stdout.printf ("%s = %s\n", column_names[i], values[i]);
        }
        stdout.printf ("\n");

        return 0;
    }

    public bool openMarlinDB()
    {
        File home_dir = File.new_for_path (Environment.get_home_dir ());
        File data_dir = home_dir.get_child(".config").get_child("marlin");

        try {
            if (!data_dir.query_exists(null))
                data_dir.make_directory_with_parents(null);
        } catch (Error err) {
            stderr.printf("Unable to create data directory %s: %s", data_dir.get_path(), err.message);
        }

        string marlin_db_path = data_dir.get_child("marlin.db").get_path();
        //The database must exists, add here the full path
        print("Database path: %s \n", marlin_db_path);
        openDB(marlin_db_path);

        return true;
    }

    private bool openDB(string dbpath){
        int rc = Sqlite.Database.open_v2(dbpath, out db, Sqlite.OPEN_READWRITE | Sqlite.OPEN_CREATE, null);

        if (rc != Sqlite.OK) {
            stderr.printf ("Can't open database: %d, %s\n", rc, db.errmsg ());
            return false;
        }

        // disable synchronized commits for performance reasons ... this is not vital
        rc = db.exec("PRAGMA synchronous=OFF");
        if (rc != Sqlite.OK)
            stdout.printf("Unable to disable synchronous mode %d, %s\n", rc, db.errmsg ());

        Sqlite.Statement stmt;
        int res = db.prepare_v2("CREATE TABLE IF NOT EXISTS tags ("
                                + "id INTEGER PRIMARY KEY, "
                                + "uri TEXT UNIQUE NOT NULL, "
                                + "color INTEGER DEFAULT 0, "
                                + "tags TEXT NULL, "
                                + "content_type TEXT, "
                                + "modified_time INTEGER DEFAULT 0, "
                                + "dir TEXT "
                                + ")", -1, out stmt);
        assert(res == Sqlite.OK);
        res = stmt.step();
        if (res != Sqlite.DONE)
            fatal("create tags table", res);

        /* TODO check result of the last sql command */
        upgrade_database ();

        return true;
    }

#if 0
    public async bool record_uri (string raw_uri, string content_type, int modified_time) {
        var uri = escape(raw_uri);

        var sql = "INSERT OR REPLACE INTO tags (uri,content_type,modified_time) VALUES ('"+uri+"','"+content_type+"',"+modified_time.to_string()+");\n";	
        //var c = "INSERT OR IGNORE INTO tags (uri,content_type,modified_time) VALUES ('"+uri+"','"+content_type+"',"+modified_time.to_string()+");\n";	

        int rc = db.exec (sql, null, null);
        if (rc != Sqlite.OK) { 
            stderr.printf ("[record_uri: SQL error]  %d, %s\n", rc, db.errmsg ());
            return false;
        }
        //stdout.printf("[Consult]: %s\n",c);

        return true;		
    }
#endif

    public async bool record_uris (Variant[] locations, string directory) {
        string sql = "";
        foreach (var location_variant in locations) {
            VariantIter iter = location_variant.iterator();

            var uri = iter.next_value ().get_string ();
            var content_type = iter.next_value ().get_string ();
            var modified_time = iter.next_value ().get_string ();
            var color = iter.next_value ().get_string ();
            sql += "INSERT OR REPLACE INTO tags (uri, content_type, color, modified_time, dir) VALUES ('%s', '%s', %s, %s, '%s');\n".printf (uri, content_type, color, modified_time, directory);
        }

        int rc = db.exec (sql, null, null);
        if (rc != Sqlite.OK) { 
            stderr.printf ("[record_uri: SQL error]  %d, %s\n", rc, db.errmsg ());
            return false;
        }
        //stdout.printf("[Consult]: %s\n",sql);

        return true;		
    }

    private string escape(string input){
        return input.replace("'", "''");
    }

    public async Variant get_uri_infos (string raw_uri)
    {
        Idle.add (get_uri_infos.callback);
        yield;
        var uri = escape (raw_uri);
        Statement stmt;

        var vb = new VariantBuilder (new VariantType ("(as)"));
        int rc = db.prepare_v2 ("select modified_time, content_type, color from tags where uri='%s'".printf (uri),
                                -1, out stmt);
        assert (rc == Sqlite.OK); 
        rc = stmt.step();

        vb.open (new VariantType ("as"));

        switch (rc) {
        case Sqlite.DONE:
            break;
        case Sqlite.ROW:
            vb.add ("s", stmt.column_text (0));
            var content_type = stmt.column_text (1);
            vb.add ("s", (content_type != null) ? content_type : "");
            vb.add ("s", stmt.column_text (2));
            break;
        default:
            printerr ("[get_uri_infos]: Error: %d, %s\n", rc, db.errmsg ());
            break;
        }

        vb.close ();
        return vb.end ();
    }

#if 0
    public async int getColor(string raw_uri)
    {
        Idle.add (getColor.callback);
        yield;
        var uri = escape(raw_uri);
        string c = "select color from tags where uri='" + uri + "'";
        Statement stmt;
        int rc = 0;
        int col, cols;
        string txt = "0";

        if ((rc = db.prepare_v2 (c, -1, out stmt, null)) == 1) {
            printerr ("[getColor]: SQL error: %d, %s\n", rc, db.errmsg ());
            return -1;
        }
        cols = stmt.column_count();
        do {
            rc = stmt.step();
            switch (rc) {
            case Sqlite.DONE:
                break;
            case Sqlite.ROW:
                for (col = 0; col < cols; col++) {
                    txt = stmt.column_text(col);
                    //print ("%s = %s\n", stmt.column_name (col), txt);
                }
                break;
            default:
                printerr ("[getColor]: Error: %d, %s\n", rc, db.errmsg ());
                break;
            }
        } while (rc == Sqlite.ROW);
        //stdout.printf("[getColor]: %s\n", txt);

        int ret = int.parse(txt);
        /* It appears that a db error return -1, we got to check the value just in case */
        if(ret == -1){
            ret = 0;
        }

        return ret;
    }
#endif

    public async bool deleteEntry(string uri)
    {
        Idle.add (deleteEntry.callback);
        yield;
        string c = "delete from tags where uri='" + uri + "'";
        int   rc = db.exec (c, null, null);

        if (rc != Sqlite.OK) { 
            stderr.printf ("[deleteEntry: SQL error]  %d, %s\n", rc, db.errmsg ());
            return false;
        }

        return true;		
    }

    public bool showTable(string table){
        stdout.printf("showTable\n");
        string consult = "select * from " + table;
        int rc = db.exec (consult, show_table_callback, null);

        if (rc != Sqlite.OK) { 
            stderr.printf ("[showTable: SQL error]: %d, %s\n", rc, db.errmsg ());
            return false;
        }

        return true;
    }

    public bool clearDB(){
        string c = "delete from tags"; 
        int   rc = db.exec (c, null, null);

        if (rc != Sqlite.OK) { 
            stderr.printf ("[clearDB: SQL error]  %d, %s\n", rc, db.errmsg ());
            return false;
        }

        return true;	
    }

    private bool has_column(string table_name, string column_name) {
        Sqlite.Statement stmt;
        int res = db.prepare_v2("PRAGMA table_info(%s)".printf(table_name), -1, out stmt);
        assert(res == Sqlite.OK);
        
        for (;;) {
            res = stmt.step();
            if (res == Sqlite.DONE) {
                break;
            } else if (res != Sqlite.ROW) {
                fatal("has_column %s".printf(table_name), res);
                
                break;
            } else {
                string column = stmt.column_text(1);
                if (column != null && column == column_name)
                    return true;
            }
        }
        
        return false;
    }

    private bool add_column(string table_name, string column_name, string column_constraints) {
        Sqlite.Statement stmt;
        int res = db.prepare_v2("ALTER TABLE %s ADD COLUMN %s %s".printf(table_name, column_name,
            column_constraints), -1, out stmt);
        assert(res == Sqlite.OK);
        
        res = stmt.step();
        if (res != Sqlite.DONE) {
            critical("Unable to add column %s %s %s: (%d) %s", table_name, column_name, column_constraints,
                res, db.errmsg());
            
            return false;
        }
        
        return true;
    }

    private void upgrade_database () {

        // version 2

        if (!has_column("tags", "content_type")) {
            message("upgrade_database: adding content_type column to tags");
            if (!add_column("tags", "content_type", "TEXT"))
                warning ("UPGRADE_ERROR");
        }
        if (!has_column("tags", "modified_time")) {
            message("upgrade_database: adding modified_time column to tags");
            if (!add_column("tags", "modified_time", "INTEGER DEFAULT 0"))
                warning ("UPGRADE_ERROR");
        }
        if (!has_column("tags", "dir")) {
            message("upgrade_database: adding dir column to tags");
            if (!add_column("tags", "dir", "TEXT"))
                warning ("UPGRADE_ERROR");
        }
    }

}

/* =============== Main ==================== */
/*void main (string[] args) {

  MarlinTags t = new MarlinTags();

  t.openMarlinDB();

  t.setColor("file:///home/jordi"	,MARLIN_RED);
  t.setColor("file:///home/dev"	,MARLIN_YELLOW);

//t.deleteEntry(File.new_for_path ("/home/dev"));	//When deleting files
//t.deleteEntry("/home/documents");

//t.clearDB();
t.showTable("tags");


// DBUS Things
print("\n\nColor for file is %i\n", 
t.getColor("file:///home/jordi"));
}*/


void on_bus_aquired (DBusConnection conn) {
    try {
        conn.register_object ("/org/elementary/marlin/db", new MarlinTags ());
    } catch (IOError e) {
        error ("Could not register service");
    }
}

void main () {
    Bus.own_name (BusType.SESSION, "org.elementary.marlin.db", BusNameOwnerFlags.NONE,
                  on_bus_aquired,
                  () => {},
                  () => error ("Could not aquire name"));

    new MainLoop ().run ();
}

