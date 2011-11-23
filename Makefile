CC	= gcc
CFLAGS	= -Wall -g
LDFLAGS	=#


TARGETS = ls

.PHONY:all clean

all:$(TARGETS)

clean:
	rm	-rf    $(TARGETS) *~
