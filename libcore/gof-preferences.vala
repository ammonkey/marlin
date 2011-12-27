/*  
 * Copyright (C) 2011 ammonkey <am.monkeyd@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 

namespace GOF {

    public static Preferences? preferences = null;

    public class Preferences : GLib.Object {
   
        public bool pref_confirm_trash = true;
        public bool confirm_trash {
            get {
                return pref_confirm_trash;
            }
            set {
                pref_confirm_trash = value;
            }
        }

        public bool pref_interpret_desktop_files = true;
        public bool interpret_desktop_files {
            get {
                return pref_interpret_desktop_files;
            }
            set {
                pref_interpret_desktop_files = value;
            }
        }

        public string pref_date_format = "iso";
        public string date_format {
            get {
                return pref_date_format;
            }
            set {
                pref_date_format = value;
            }
        }

        public class Preferences () {
        
        }

        public static Preferences get_default () {
            if (preferences != null)
                return preferences;

            preferences = new Preferences ();
            return preferences;
        }
    }
}