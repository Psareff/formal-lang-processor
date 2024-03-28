CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS := $(shell pkg-config --libs gtk+-3.0)
CC = gcc

SRCDIR=src
OUTDIR=out
LDDIR=$(SRCDIR)/lib

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%.o, $(SRCS))
LDLIBS=$(patsubst $(LDDIR)%.c, $(OUTDIR)/%.so, $(SRCS))

INCLUDE=-Iinclude

$(OUTDIR)/%.o : $(SRCDIR)/%.c Makefile
	$(CC) -DRELEASE -DLEX_DB -c $< $(INCLUDE) $(CFLAGS) -o $@ $(LDFLAGS)

PROJ_NAME=codEditor

all: $(OBJS) 
	$(CC) $^ $(CFLAGS) -o  $(PROJ_NAME) $(LDFLAGS)

configure:
	mkdir -p out
	mkdir -p shared

clean:
	rm -rf out/* 2> /dev/null && rm -f $(PROJ_NAME) 2> /dev/null
	rmdir out shared 2> /dev/null

