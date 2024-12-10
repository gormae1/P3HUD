# P3HUD
Recreation of the Persona 3 HUD in GTK! Displays moon information in the top left corner.
![Screenshot of usage](https://raw.githubusercontent.com/gormae1/P3HUD/refs/heads/main/hud_usage.png)

# Requirements 
- libgtk3
- libcairo
(both of the above can be easily installed using the gnome-devel package)
- A window manager that supports transparency (compositiing), e.g. Gnome3
- skyfield (https://pypi.org/project/skyfield/)
- python3

# Usage
- To compile:
```
gcc p3hud.c -o p3hud -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0 -I/usr/include/cairo -I/usr/include/libdrm -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/fribidi -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/uuid -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
```
- To run:
```
./p3hud <path to moon3.py> <path to sprites folder>
```
example: `./p3hud ./ ./sprites/`

# Acknowledgements
All sprites belong to ATLUS.
