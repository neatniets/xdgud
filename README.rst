xdgud, xdg-user-dir
===================
`xdgud` allows you to execute commands in the XDG user directories while still
maintaining a relation with the current directory.
For example,
        `xdgud TEMPLATES cp boiler-plate.c ./`
copies the template file "boiler-plate.c" to the current directory.
When executed with only the XDG directory name, the absolute path to that
directory will be printed.

Under all circumstances, the command will fail if the directory isn't found.
No arbitrary fallback directories are used.

See the manpage for more details.

Installation
============
Configure "config.mk" to suit your system.
Then, `sudo make install`.
