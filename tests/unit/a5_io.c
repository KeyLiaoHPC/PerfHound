/**
 * A5 — IO dump tree: directory layout, ph_dump flush, CSV header consistency.
 */
#include "perfhound.h"
#include "../common/ph_test.h"

#include <string.h>

int
main(void)
{
    const char *id = "A5";
    char tmpdir[256];
    char run_path[512];
    char rec_path[512];
    char ctag_path[512];
    char rankmap_path[512];
    long lines_before;
    long lines_after;

    PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                    "failed to create temp directory");

    PH_TEST_REQUIRE(id, ph_init(tmpdir) == 0, "ph_init failed");
    ph_commit();
    ph_read(1, 1, 0.0);

    PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                sizeof(run_path)) == 0,
                    "run directory missing after commit");

    snprintf(ctag_path, sizeof(ctag_path), "%s/ctag.csv", run_path);
    snprintf(rankmap_path, sizeof(rankmap_path), "%s/rankmap.csv", run_path);
    PH_TEST_REQUIRE(id, ph_test_path_exists(ctag_path), "ctag.csv missing");
    PH_TEST_REQUIRE(id, ph_test_path_exists(rankmap_path), "rankmap.csv missing");

    PH_TEST_REQUIRE(id, ph_test_find_rec_file(run_path, rec_path,
                                              sizeof(rec_path)) == 0,
                    "record csv missing");
    lines_before = ph_test_file_lines(rec_path);
    PH_TEST_REQUIRE(id, lines_before >= 1, "record header missing");

    ph_read(1, 2, 0.0);
    ph_dump();

    lines_after = ph_test_file_lines(rec_path);
    PH_TEST_REQUIRE(id, lines_after > lines_before,
                    "ph_dump did not append records (%ld -> %ld)",
                    lines_before, lines_after);

    PH_TEST_REQUIRE(id, ph_test_path_exists(tmpdir), "project run_info root missing");
    ph_finalize();

    PH_TEST_PASS(id);
}
