/**
 * B3 — Multi-tag phases: init / compute / epilogue with distinct gid,pid pairs.
 */
#include "perfhound.h"
#include "../common/ph_test.h"

#include <string.h>

int
main(void)
{
    const char *id = "B3";
    char tmpdir[256];
    char run_path[512];
    char rec_path[512];
    char ctag_path[512];

    PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                    "failed to create temp directory");

    PH_TEST_REQUIRE(id, ph_init(tmpdir) == 0, "ph_init failed");

    PH_TEST_REQUIRE(id, ph_set_tag(30, 0, "InitPhase") == 0, "tag init failed");
    PH_TEST_REQUIRE(id, ph_set_tag(31, 0, "ComputePhase") == 0, "tag compute failed");
    PH_TEST_REQUIRE(id, ph_set_tag(32, 0, "EpiloguePhase") == 0, "tag epilogue failed");

    ph_commit();

    ph_read(30, 1, 0.0);
    ph_test_busy_work(10000);
    ph_read(30, 2, 0.0);

    ph_read(31, 1, 0.0);
    ph_test_busy_work(100000);
    ph_read(31, 2, 0.0);

    ph_read(32, 1, 0.0);
    ph_read(32, 2, 0.0);
    ph_finalize();

    PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                sizeof(run_path)) == 0,
                    "run directory missing");
    snprintf(ctag_path, sizeof(ctag_path), "%s/ctag.csv", run_path);
    PH_TEST_REQUIRE(id, ph_test_ctag_has(ctag_path, 30, 0, "InitPhase"),
                    "missing init phase tag");
    PH_TEST_REQUIRE(id, ph_test_ctag_has(ctag_path, 31, 0, "ComputePhase"),
                    "missing compute phase tag");
    PH_TEST_REQUIRE(id, ph_test_ctag_has(ctag_path, 32, 0, "EpiloguePhase"),
                    "missing epilogue phase tag");

    PH_TEST_REQUIRE(id, ph_test_find_rec_file(run_path, rec_path,
                                              sizeof(rec_path)) == 0,
                    "record csv missing");
    PH_TEST_REQUIRE(id, ph_test_rec_has_tag(rec_path, 30, 1)
                    && ph_test_rec_has_tag(rec_path, 31, 1)
                    && ph_test_rec_has_tag(rec_path, 32, 1),
                    "missing phase start samples in record");

    PH_TEST_PASS(id);
}
