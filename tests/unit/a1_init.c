/**
 * A1 — Init / Finalize: minimal ph_init and ph_finalize lifecycle.
 */
#include "perfhound.h"
#include "../common/ph_test.h"

#include <string.h>

int
main(void)
{
    const char *id = "A1";
    char tmpdir[256];
    char run_path[512];
    int err;

    PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                    "failed to create temp directory");

    err = ph_init(tmpdir);
    PH_TEST_REQUIRE(id, err == 0, "ph_init returned %d", err);
    PH_TEST_REQUIRE(id, ph_test_path_exists(tmpdir), "output root missing");

    ph_commit();
    ph_read(1, 1, 0.0);
    ph_finalize();

    PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                sizeof(run_path)) == 0,
                    "run directory not created");

    PH_TEST_PASS(id);
}
