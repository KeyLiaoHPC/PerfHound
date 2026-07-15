# Top-level PerfHound entry (replaces unused TPBench leftover).
.PHONY: all probe check clean

all: probe

probe:
	$(MAKE) -C src/probe libph.so
	@mkdir -p src/probe/lib
	cp -f src/probe/libph.so src/probe/lib/

check:
	$(MAKE) -C tests check

clean:
	$(MAKE) -C src/probe clean
	$(MAKE) -C tests clean
