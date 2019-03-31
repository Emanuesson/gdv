/* Copyright 2019 Emanuel Schmidt
 *
 * This software is licensed under the GNU Lesser General Public License
 * (version 2.1 or later).  See the COPYING file in this distribution.
 */


class MyApplication : Gtk.Application {
        protected override void activate () {
                var window = new Gtk.ApplicationWindow (this);
                var label = new Gtk.Label ("Hello GNOME!");
                window.add (label);
                window.set_title ("Welcome to GNOME");
                window.set_default_size (200, 100);
                window.show_all ();
        }
}

int main (string[] args) {
        return new MyApplication ().run (args);
}
