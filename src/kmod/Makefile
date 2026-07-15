# PerfHound unified PMU kernel module build entry.
# Selects x86/ or aarch64/ by host arch; output is always ph_enable_pmu.ko.

ARCH_RAW := $(shell uname -m)
ifeq ($(ARCH_RAW),x86_64)
  PH_KMOD_ARCH := x86
else ifeq ($(ARCH_RAW),aarch64)
  PH_KMOD_ARCH := aarch64
else
  $(error *** [PH-kmod] Unsupported architecture: $(ARCH_RAW); only x86_64 and aarch64 are supported)
endif

PH_KMOD_DIR := $(CURDIR)/$(PH_KMOD_ARCH)
PH_KMOD_KO  := $(PH_KMOD_DIR)/ph_enable_pmu.ko

.PHONY: all clean check

all:
	$(MAKE) -C $(PH_KMOD_DIR) all
	@echo ""
	@echo "*** [PH-kmod] Built: src/kmod/$(PH_KMOD_ARCH)/ph_enable_pmu.ko"
	@echo "*** [PH-kmod] Load manually (root required):"
	@echo "*** [PH-kmod]   sudo insmod src/kmod/$(PH_KMOD_ARCH)/ph_enable_pmu.ko"
	@echo "*** [PH-kmod] Unload: sudo rmmod ph_enable_pmu"
	@echo "*** [PH-kmod] Verify: cd src/kmod && make check"

clean:
	$(MAKE) -C $(PH_KMOD_DIR) clean

# make check does not insmod; it verifies the module is already loaded.
check:
	@bash check/check_loaded.sh "$(PH_KMOD_ARCH)"
	@bash check/check_$(PH_KMOD_ARCH).sh
	@echo "*** [PH-kmod] check passed"
