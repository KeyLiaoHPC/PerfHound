#!/bin/sh

CC = gcc
PERFHOUND = "PERFHOUND"
PAPI = PAPI
PROF ?= $(PERFHOUND)
NMEASURE ?= 100
CFLAGS=-O2

$(info PROF is $(PROF))

ifeq ($(PROF),PERFHOUND)
PROF_FLAGS = -I$(PERFHOUND)/include
# Modify this for appropriate mode you want.
# -lpfhprobe_ts: Timestamp mode
# -lpfhprobe_ev: 4-event mode
# -lpfhprobe_evx: Extended-event  mode (12 for AArch64, 8 for Cascade Lake X)
PROF_LINK = -L$(PERFHOUND)/lib -lpfhprobe_ts

else 
ifeq ($(PROF),PAPI)
PROF_FLAGS = -I$(PAPI)/include
PROF_LINK = -L$(PAP)/lib -lpapi

else
PROF_FLAGS = 
PROF_LINK = 
endif

endif

CFLAGS += $(PROF_FLAGS)
LDFLAGS += $(PROF_LINK)

# Specify profiler, PAPI or PERFHOUND

# Modify this for kernel's length
NADD ?= 10

PRED = -DNADD=$(NADD) -DNMEASURE=$(NMEASURE) -D$(PROF)

.PHONY: clean all

all: test-1

test-1: test1_3regs_add

test1_3regs_add : test1_3regs_add.c
	$(CC) $(CFLAGS) $(PRED) -funroll-all-loops -o $@_$(PROF).x $^ $(LDFLAGS)
	$(CC) $(CFLAGS) $(PRED) -c -funroll-all-loops -S -fverbose-asm -o $@_$(PROF).s $^ 

clean:
	rm -r ./*.x  ./*.s
