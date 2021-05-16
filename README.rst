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
To install:
        `sudo make install`

**NOTE:** a symbolic link to `xdgud` called `xdg-user-dir` is also made and
installed by default.
The manpage explains the details of the difference when invoking the command
under the different names.
If this is not desirable, simply remove that line from the `install` command in
the Makefile.
