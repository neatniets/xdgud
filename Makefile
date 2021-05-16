.POSIX:

.SUFFIXES:
.SUFFIXES: .c .o

include config.mk
BIN=$(DESTDIR)$(PREFIX)/bin
MAN=$(DESTDIR)$(MANPREFIX)/man1

SRC=xdgud.c path.c printerr.c xdg.c
OBJ=$(SRC:.c=.o)


all: xdgud xdg-user-dir

clean:
	rm -f xdgud xdg-user-dir $(OBJ)
install: all
	mkdir -p $(BIN)
	cp -f xdgud $(BIN)
	chmod 0755 $(BIN)/xdgud
	cp -f xdg-user-dir $(BIN)
	mkdir -p $(MAN)
	cp -f xdgud.1 $(MAN)
uninstall: clean
	rm -f $(BIN)/xdgud
	rm -f $(BIN)/xdg-user-dir
	rm -f $(MAN)/xdgud.1

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
