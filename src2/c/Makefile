
CFLAGS+=-Werror -Wall
CFLAGS+=-g

all: capture

capture: capture.o hexdump.o
	$(LINK.c) -o $@ $^


clean:
	rm -f *.o capture
