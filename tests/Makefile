#!/bin/sh
CC = gcc
MPICC = mpicc
CFLAGS = -O2 -fno-builtin -DUSE_MPI 
# 
#
STREAM_FLAGS = -DSTREAM_ARRAY_SIZE=16384 -DNTIMES=10000
VARAPI_FLAGS = -DUSE_VARAPI #-DVT_MODE_SHORT -DVT_UEV_1
LDFLAGS = 

VPATH = ../pfhprobe

.PHONY: test1 test2 test3 clean

test1: test1_asm_x86.x test1_asm_x86_mpi.x

test2: test2_asm_armv8.x test1_asm_armv8_mpi.x

test1_asm_x86_ts.x: test1_asm_x86.c
	$(CC) $(CFLAGS) -o ^@ $^ -lpfh_ts 
test1_asm_x86_evx.x: test1_asm_x86.c
	$(CC) $(CFLAGS) -o ^@ $^ -lpfh_evx
test1_asm_x86_ts_mpi.x: test1_asm_x86.c
	$(MPICC) $(CFLAGS) -o ^@ $^ -lpfh_ts 
test1_asm_x86_evx_mpi.x: test1_asm_x86.c
	$(MPICC) $(CFLAGS) -o ^@ $^ -lpfh_evx 

test2_asm_armv8_ts.x: test2_asm_armv8.c
test2_asm_armv8_evx.x: test2_asm_armv8.c
test2_asm_armv8_ts_mpi.x: test2_asm_armv8.c
test2_asm_armv8_evx_mpi.x: test2_asm_armv8.c

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
