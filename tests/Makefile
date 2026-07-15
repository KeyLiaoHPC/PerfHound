# PerfHound functional test harness.
# Run from repo root: make check
# Or here: make check [TESTS=A2,B1]

CC       ?= gcc
MPICC    ?= mpicc
ARCH     := $(shell uname -m)

PROBE_DIR  := $(abspath ../src/probe)
PROBE_INC  := $(PROBE_DIR)/include
PROBE_LIB  := $(PROBE_DIR)/lib
TEST_LIB   := $(CURDIR)/lib
BIN_DIR    := $(CURDIR)/bin
COMMON_DIR := $(CURDIR)/common

CFLAGS  := -O2 -Wall -I$(PROBE_INC) -I$(COMMON_DIR)
LDFLAGS := -L$(PROBE_LIB) -Wl,-rpath,$(PROBE_LIB)
EV_LDFLAGS := -L$(TEST_LIB) -Wl,-rpath,$(TEST_LIB)

# Both papi.h include and -lpapi link must succeed.
HAVE_PAPI := $(shell printf '#include <papi.h>\nint main(void){return 0;}\n' | \
	$(CC) -x c - -lpapi -o $(CURDIR)/.papi_probe >/dev/null 2>&1 && echo yes || echo no)

# Both mpi.h include and -lmpi link must succeed.
HAVE_MPI := $(shell printf '#include <mpi.h>\nint main(void){return 0;}\n' | \
	$(CC) -x c - -lmpi -o $(CURDIR)/.mpi_probe >/dev/null 2>&1 && echo yes || echo no)

MPI_LAUNCHER := $(shell command -v mpirun 2>/dev/null || command -v mpiexec 2>/dev/null || true)

# PHASM timer/PMU path requires the unified kmod to be loaded.
HAVE_KMOD := $(shell test -d /sys/module/ph_enable_pmu && echo yes || echo no)

ifeq ($(HAVE_PAPI),yes)
  PROBE_API := PAPI
  PROBE_MAKE_EXTRA := PAPI=$(PAPI)
else
  PROBE_API := PHASM
  PROBE_MAKE_EXTRA :=
endif

MPI_LDFLAGS := -lmpi
ifeq ($(ARCH),aarch64)
  MPI_LDFLAGS :=
endif

UTIL_OBJ := $(BIN_DIR)/ph_test_util.o

SERIAL_TESTS := A1 A2 A3 A4 A5 A6 B1 B2 B3
MPI_TESTS    := AM1 AM2 AM3 AM4 BM1 BM2
SERIAL_BINS  := $(addprefix $(BIN_DIR)/,$(SERIAL_TESTS))
MPI_BINS     := $(addprefix $(BIN_DIR)/,$(MPI_TESTS))

.PHONY: all check clean probe-lib probe-lib-ev-stamp probe-restore \
	$(SERIAL_TESTS) $(MPI_TESTS)

all: probe-lib $(SERIAL_BINS) $(if $(filter yes,$(HAVE_MPI)),$(MPI_BINS),)

probe-lib:
	@echo "*** [PH-tests] HAVE_PAPI=$(HAVE_PAPI) HAVE_MPI=$(HAVE_MPI) HAVE_KMOD=$(HAVE_KMOD) PROBE_API=$(PROBE_API)"
	$(MAKE) -C $(PROBE_DIR) PH_API=$(PROBE_API) $(PROBE_MAKE_EXTRA) libph.so
	@mkdir -p $(PROBE_LIB)
	cp -f $(PROBE_DIR)/libph.so $(PROBE_LIB)/
ifeq ($(HAVE_MPI),yes)
	$(MAKE) -C $(PROBE_DIR) PH_API=$(PROBE_API) $(PROBE_MAKE_EXTRA) libphmpi.so
	cp -f $(PROBE_DIR)/libphmpi.so $(PROBE_LIB)/
endif

probe-lib-ev-stamp:
	@mkdir -p $(TEST_LIB)
	@if $(MAKE) -C $(PROBE_DIR) PH_API=$(PROBE_API) PH_EVMODE=EV $(PROBE_MAKE_EXTRA) libph.so \
		>/dev/null 2>&1; then \
		cp -f $(PROBE_DIR)/libph.so $(TEST_LIB)/libph_ev.so; \
		touch $@; \
	else \
		echo "*** [PH-tests] EV library build failed; A4/B2 may SKIP"; \
		touch $@; \
	fi

probe-lib-ev: probe-lib-ev-stamp

probe-restore:
	$(MAKE) -C $(PROBE_DIR) PH_API=$(PROBE_API) $(PROBE_MAKE_EXTRA) libph.so
	@mkdir -p $(PROBE_LIB)
	cp -f $(PROBE_DIR)/libph.so $(PROBE_LIB)/
ifeq ($(HAVE_MPI),yes)
	$(MAKE) -C $(PROBE_DIR) PH_API=$(PROBE_API) $(PROBE_MAKE_EXTRA) libphmpi.so
	cp -f $(PROBE_DIR)/libphmpi.so $(PROBE_LIB)/ 2>/dev/null || true
endif

$(UTIL_OBJ): $(COMMON_DIR)/ph_test_util.c $(COMMON_DIR)/ph_test.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

define BUILD_SERIAL
$(BIN_DIR)/$(1): unit/$(2) $(UTIL_OBJ) | probe-lib
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $$@ unit/$(2) $(UTIL_OBJ) $(LDFLAGS) -lph
endef

define BUILD_SERIAL_INTEG
$(BIN_DIR)/$(1): integ/$(2) $(UTIL_OBJ) | probe-lib
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $$@ integ/$(2) $(UTIL_OBJ) $(LDFLAGS) -lph
endef

$(eval $(call BUILD_SERIAL,A1,a1_init.c))
$(eval $(call BUILD_SERIAL,A2,a2_timing.c))
$(eval $(call BUILD_SERIAL,A3,a3_tags.c))
$(eval $(call BUILD_SERIAL,A5,a5_io.c))
$(eval $(call BUILD_SERIAL,A6,a6_api_order.c))
$(eval $(call BUILD_SERIAL_INTEG,B1,b1_busy_loop.c))
$(eval $(call BUILD_SERIAL_INTEG,B3,b3_multi_phase.c))

$(BIN_DIR)/A4: unit/a4_counters.c $(UTIL_OBJ) probe-lib-ev-stamp
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ unit/a4_counters.c $(UTIL_OBJ) \
		$(if $(wildcard $(TEST_LIB)/libph_ev.so),$(EV_LDFLAGS) -lph_ev,) \
		$(if $(wildcard $(TEST_LIB)/libph_ev.so),,$(LDFLAGS) -lph)
	@if [ ! -f "$(TEST_LIB)/libph_ev.so" ]; then \
		echo "*** [PH-tests] A4 built against TS lib; will SKIP at runtime"; \
	fi

$(BIN_DIR)/B2: integ/b2_work_events.c $(UTIL_OBJ) probe-lib-ev-stamp
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ integ/b2_work_events.c $(UTIL_OBJ) \
		$(if $(wildcard $(TEST_LIB)/libph_ev.so),$(EV_LDFLAGS) -lph_ev,) \
		$(if $(wildcard $(TEST_LIB)/libph_ev.so),,$(LDFLAGS) -lph)

ifeq ($(HAVE_MPI),yes)
define BUILD_MPI
$(BIN_DIR)/$(1): unit/$(2) $(UTIL_OBJ) | probe-lib
	@mkdir -p $(BIN_DIR)
	$(MPICC) $(CFLAGS) -o $$@ unit/$(2) $(UTIL_OBJ) $(LDFLAGS) -lphmpi $(MPI_LDFLAGS)
endef

define BUILD_MPI_INTEG
$(BIN_DIR)/$(1): integ/$(2) $(UTIL_OBJ) | probe-lib
	@mkdir -p $(BIN_DIR)
	$(MPICC) $(CFLAGS) -o $$@ integ/$(2) $(UTIL_OBJ) $(LDFLAGS) -lphmpi $(MPI_LDFLAGS)
endef

$(eval $(call BUILD_MPI,AM1,am1_mpi_init.c))
$(eval $(call BUILD_MPI,AM2,am2_mpi_tags.c))
$(eval $(call BUILD_MPI,AM3,am3_mpi_io.c))
$(eval $(call BUILD_MPI,AM4,am4_mpi_fastread.c))
$(eval $(call BUILD_MPI_INTEG,BM1,bm1_mpi_loop.c))
$(eval $(call BUILD_MPI_INTEG,BM2,bm2_mpi_phases.c))
endif

check: all
	@chmod +x run_all.sh
	@PROBE_API=$(PROBE_API) HAVE_KMOD=$(HAVE_KMOD) \
		bash run_all.sh "$(BIN_DIR)" "$(MPI_LAUNCHER)" 2; rc=$$?; \
	$(MAKE) probe-restore; exit $$rc

clean:
	rm -rf $(BIN_DIR) $(TEST_LIB) .papi_probe .mpi_probe
	$(MAKE) -C $(PROBE_DIR) clean
