#!/bin/sh
CC = mpicc
CFLAGS = -O3 -fno-builtin -DUSE_MPI
# 
#VARAPI = /chpc/home/stf-keyliao-a/proj/Vartect/varapi
VARAPI = $(PWD)/..
#
STREAM_FLAGS = -DSTREAM_ARRAY_SIZE=16777216 -DNTIMES=50
VARAPI_FLAGS = -DUSE_VARAPI
LDFLAGS = 

VPATH = $(VARAPI)

.PHONY: clean

stream.x: stream.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o  ^@ $^

stream_varapi.x: stream_varapi.o varapi.o file_op.o varapi.o
	$(CC) $(CFLAGS) $(STREAM_FLAGS) -o $@ $^ 

st_mpi_varapi.x: st_mpi_varapi.o varapi.o file_op.o varapi.o vt_mpi.o
	$(CC) $(CFLAGS) $(STREAM_FLAGS) -o $@ $^ 

st_mpi_varapi.o: stream_mpi.c
	$(CC) $(CFLAGS) $(STREAM_FLAGS) $(VARAPI_FLAGS) -I$(VARAPI) -c -o $@ $< 

stream_varapi.o: stream.c 
	$(CC) $(CFLAGS) $(STREAM_FLAGS) $(VARAPI_FLAGS) -I$(VARAPI) -c -o $@ $< 

%.o: %.c
	$(CC) $(CFLAGS) $(STREAM_FLAGS) $(VARAPI_FLAGS) -I$(VARAPI) -c $< 

clean:
	rm -rf *.o stream_varapi.x stream.x
	rm -rf ./data
