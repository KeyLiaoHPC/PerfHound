/**
 * A3 — Tags: user ph_set_tag entries appear in ctag.csv and record samples.
 */
#include "perfhound.h"
#include "../common/ph_test.h"

#include <string.h>

int
main(void)
{
    const char *id = "A3";
    char tmpdir[256];
    char run_path[512];
    char rec_path[512];
    char ctag_path[512];

    PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                    "failed to create temp directory");

    PH_TEST_REQUIRE(id, ph_init(tmpdir) == 0, "ph_init failed");
    PH_TEST_REQUIRE(id, ph_set_tag(1, 0, "UserGroup") == 0, "set_tag group failed");
    PH_TEST_REQUIRE(id, ph_set_tag(1, 1, "UserStart") == 0, "set_tag start failed");

    ph_commit();
    ph_read(1, 1, 0.0);
    ph_finalize();

    PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                sizeof(run_path)) == 0,
                    "run directory missing");
    snprintf(ctag_path, sizeof(ctag_path), "%s/ctag.csv", run_path);
    PH_TEST_REQUIRE(id, ph_test_ctag_has(ctag_path, 1, 0, "UserGroup"),
                    "ctag missing user group");
    PH_TEST_REQUIRE(id, ph_test_ctag_has(ctag_path, 1, 1, "UserStart"),
                    "ctag missing user start");

    PH_TEST_REQUIRE(id, ph_test_find_rec_file(run_path, rec_path,
                                              sizeof(rec_path)) == 0,
                    "record csv missing");
    PH_TEST_REQUIRE(id, ph_test_rec_has_tag(rec_path, 1, 1),
                    "record missing user sample tag");

    PH_TEST_PASS(id);
}
