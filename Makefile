.POSIX:

.SUFFIXES:
.SUFFIXES: .c .o

include config.mk

SRC=xdgud.c
OBJ=$(SRC:.c=.o)

all: xdgud

clean:
	rm -f xdgud $(OBJ)

xdgud: xdgud.o
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
