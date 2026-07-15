/**
 * A2 — Timing read: monotonic cycle/wall timestamps after ph_read samples.
 */
#include "perfhound.h"
#include "../common/ph_test.h"

#include <string.h>

int
main(void)
{
    const char *id = "A2";
    char tmpdir[256];
    char run_path[512];
    char rec_path[512];
    char ctag_path[512];
    int rows;
    int count;
    int i;

    PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                    "failed to create temp directory");

    PH_TEST_REQUIRE(id, ph_init(tmpdir) == 0, "ph_init failed");
    ph_commit();

    for (i = 0; i < 4; i++) {
        ph_read(1, 1, 0.0);
        ph_test_busy_work(5000);
    }
    ph_finalize();

    PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                sizeof(run_path)) == 0,
                    "run directory missing");
    PH_TEST_REQUIRE(id, ph_test_find_rec_file(run_path, rec_path,
                                              sizeof(rec_path)) == 0,
                    "record csv missing");

    PH_TEST_REQUIRE(id, ph_test_rec_monotonic_times(rec_path, &rows) == 0,
                    "timestamps not monotonic or all zero");
    PH_TEST_REQUIRE(id, ph_test_rec_count_user_samples(rec_path, 1, &count) == 0
                    && count >= 2,
                    "expected at least 2 user samples, got %d", count);
    PH_TEST_REQUIRE(id, ph_test_rec_has_tag(rec_path, 0, 1),
                    "missing internal start marker (0,1)");
    PH_TEST_REQUIRE(id, ph_test_rec_has_tag(rec_path, 0, 2),
                    "missing internal end marker (0,2)");

    snprintf(ctag_path, sizeof(ctag_path), "%s/ctag.csv", run_path);
    PH_TEST_REQUIRE(id, ph_test_ctag_has(ctag_path, 0, 1, NULL),
                    "ctag missing (0,1)");

    PH_TEST_PASS(id);
}
