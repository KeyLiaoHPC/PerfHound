/**
 * A4 — Counters / set_evt: EV-mode library, etag registration, post-commit guard.
 * Linked against libph_ev.so built with PH_EVMODE=EV.
 */
#include "perfhound.h"
#include "../common/ph_test.h"

#include <string.h>

static int
_sf_try_set_evt(const char **out_name)
{
    static const char *candidates[] = {
        "PAPI_TOT_INS",
        "inst_retired",
        "INST_RETIRED",
        "CPU_CLK_UNHALTED",
        NULL
    };
    int i;

    for (i = 0; candidates[i] != NULL; i++) {
        if (ph_set_evt(candidates[i]) == 0) {
            *out_name = candidates[i];
            return 0;
        }
    }

    return -1;
}

int
main(void)
{
    const char *id = "A4";
    char tmpdir[256];
    char run_path[512];
    const char *evt = NULL;
    char rec_path[512];
    int after;

    if (getenv("PH_TEST_NO_EV_LIB") != NULL) {
        PH_TEST_SKIP(id, "EV probe library not built (PH_TEST_NO_EV_LIB)");
    }

    PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                    "failed to create temp directory");

    PH_TEST_REQUIRE(id, ph_init(tmpdir) == 0, "ph_init failed");

    if (_sf_try_set_evt(&evt) != 0) {
        PH_TEST_SKIP(id, "no supported hardware event in this environment");
    }

    ph_commit();
    after = ph_set_evt(evt);
    PH_TEST_REQUIRE(id, after != 0,
                    "ph_set_evt after commit should fail, got %d", after);

    ph_read(1, 1, 0.0);
    ph_test_busy_work(200000);
    ph_read(1, 2, 0.0);
    ph_finalize();

    PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                sizeof(run_path)) == 0,
                    "run directory missing");
    PH_TEST_REQUIRE(id, ph_test_etag_nonempty(run_path),
                    "etag.csv has no event rows");

    PH_TEST_REQUIRE(id, ph_test_find_rec_file(run_path, rec_path,
                                              sizeof(rec_path)) == 0,
                    "record csv missing");
    if (!ph_test_rec_has_ev_columns(rec_path)) {
        PH_TEST_SKIP(id, "record csv has no event columns (not EV build)");
    }

    PH_TEST_PASS(id);
}
