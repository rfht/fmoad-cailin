CC =		cc
CFLAGS =	-c -fPIC -Wall -Werror -O0 -g -I/usr/local/include
LDFLAGS =	-shared -L/usr/local/lib -lopenal -lvorbisfile -lalut
RM =		rm -f
TARGET_LIB =	libfmodstudio.so
SRCS =		fmodstudio.c al.c

.PHONY: all
all: libfmodstudio.so

libfmodstudio.so: fmodstudio.o al.o
	$(CC) $(LDFLAGS) -o libfmodstudio.so fmodstudio.o al.o

fmodstudio.o:
	$(CC) $(CFLAGS) fmodstudio.c

al.o:
	$(CC) $(CFLAGS) al.c

.PHONY: clean
clean:
	$(RM) libfmodstudio.so fmodstudio.o al.o
