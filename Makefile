.POSIX:

.SUFFIXES:
.SUFFIXES: .c .o

include config.mk

SRC=xdgud.c path.c printerr.c
OBJ=$(SRC:.c=.o)

all: xdgud xdg-user-dir

clean:
	rm -f xdgud xdg-user-dir $(OBJ)

xdgud: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)
xdg-user-dir: xdgud
	ln -s xdgud $@

xdgud.o: path.h
path.o: path.h
printerr.o: printerr.h
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: all clean
