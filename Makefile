.POSIX:

.SUFFIXES:
.SUFFIXES: .c .o

include config.mk

SRC=xdgud.c path.c printerr.c xdg.c
OBJ=$(SRC:.c=.o)

all: xdgud xdg-user-dir

clean:
	rm -f xdgud xdg-user-dir $(OBJ)

xdgud: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)
xdg-user-dir: xdgud
	ln -s xdgud $@

xdgud.o: path.h printerr.h xdg.h
path.o: path.h printerr.h
printerr.o: printerr.h
xdg.o: xdg.h printerr.h path.h
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: all clean
