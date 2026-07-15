/**
 * B1 — Instrumented busy loop: start/end pair, delta timing, uval propagation.
 */
#include "perfhound.h"
#include "../common/ph_test.h"

#include <string.h>

int
main(void)
{
    const char *id = "B1";
    char tmpdir[256];
    char run_path[512];
    char rec_path[512];
    long long dcy;
    long long dns;
    double uval;
    unsigned long iters = 500000;
    int i;

    PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                    "failed to create temp directory");

    PH_TEST_REQUIRE(id, ph_init(tmpdir) == 0, "ph_init failed");
    PH_TEST_REQUIRE(id, ph_set_tag(10, 0, "LoopGroup") == 0, "set_tag failed");
    PH_TEST_REQUIRE(id, ph_set_tag(10, 1, "LoopStart") == 0, "set_tag failed");
    PH_TEST_REQUIRE(id, ph_set_tag(10, 2, "LoopEnd") == 0, "set_tag failed");

    ph_commit();
    ph_read(10, 1, (double)iters);

    for (i = 0; i < 8; i++) {
        ph_test_busy_work(iters);
    }

    ph_read(10, 2, (double)iters);
    ph_finalize();

    PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                sizeof(run_path)) == 0,
                    "run directory missing");
    PH_TEST_REQUIRE(id, ph_test_find_rec_file(run_path, rec_path,
                                              sizeof(rec_path)) == 0,
                    "record csv missing");

    PH_TEST_REQUIRE(id, ph_test_rec_delta_for_tag(rec_path, 10, 1, &dcy, &dns) == 0,
                    "missing loop start/end pair");
    PH_TEST_REQUIRE(id, dcy > 0 && dns > 0,
                    "unreasonable timing delta cy=%lld ns=%lld", dcy, dns);

    PH_TEST_REQUIRE(id, ph_test_rec_uval_at_tag(rec_path, 10, 1, &uval) == 0,
                    "missing uval at loop start");
    PH_TEST_REQUIRE(id, uval == (double)iters,
                    "uval mismatch: expected %lu got %f", iters, uval);

    PH_TEST_PASS(id);
}
