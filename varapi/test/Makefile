#!/bin/sh
CC = mpicc
CFLAGS = -O3 -fno-builtin
# 
VARAPI = /chpc/home/stf-keyliao-a/proj/Vartect/varapi/ 
#
STREAM_FLAGS = -DSTREAM_ARRAY_SIZE=16777216 -DNTIMES=50
VARAPI_FLAGS = -DUSE_VARTECT

VPATH = $(VARAPI)

stream.x: stream.c

stream_varapi.x: stream.c varapi.o
	$(CC) $(CFLAGS) $(STREAM_FLAGS) $(VARAPI_FLAGS) $^ $@

%.o: %.c
	$(CC) $(CFLAGS) -I$(VARAPI)/include -c $<