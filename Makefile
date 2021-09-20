CC =		cc
CFLAGS =	-c -fPIC -Wall -Werror -O2 -g -I/usr/local/include
LDFLAGS =	-shared -L/usr/local/lib -lopenal -lvorbisfile -lalut
RM =		rm -f
TARGET_LIB =	libfmodstudio.so
SRCS =		fmodstudio.c

.PHONY: all
all: libfmodstudio.so

libfmodstudio.so: fmodstudio.o
	$(CC) $(LDFLAGS) -o libfmodstudio.so fmodstudio.o

fmodstudio.o:
	$(CC) $(CFLAGS) fmodstudio.c

.PHONY: clean
clean:
	$(RM) libfmodstudio.so fmodstudio.o
