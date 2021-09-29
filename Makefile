CC =		cc
CFLAGS =	-c -fPIC -std=c99 -Wall -Werror -O0 -g -I/usr/local/include
LDFLAGS =	-shared -L/usr/local/lib -lopenal -lvorbisfile -lalut -ljson-c
RM =		rm -f
TARGET_LIB =	libfmodstudio.so
SRCS =		fmoad-cailin.c al.c

.PHONY: all
all: libfmodstudio.so

libfmodstudio.so: fmoad-cailin.o al.o
	$(CC) $(LDFLAGS) -o libfmodstudio.so fmoad-cailin.o al.o

fmodstudio.o:
	$(CC) $(CFLAGS) fmoad-cailin.c

al.o:
	$(CC) $(CFLAGS) al.c

.PHONY: clean
clean:
	$(RM) libfmodstudio.so fmoad-cailin.o al.o
