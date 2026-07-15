/**
 * A6 — API order guard: valid call sequence and reject set_evt after commit.
 */
#include "perfhound.h"
#include "../common/ph_test.h"

#include <string.h>

int
main(void)
{
    const char *id = "A6";
    char tmpdir[256];
    int after;

    PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                    "failed to create temp directory");

    PH_TEST_REQUIRE(id, ph_init(tmpdir) == 0, "ph_init failed");
    PH_TEST_REQUIRE(id, ph_set_tag(2, 0, "Phase") == 0, "set_tag failed");

    ph_commit();
    after = ph_set_evt("PAPI_TOT_INS");
    PH_TEST_REQUIRE(id, after != 0,
                    "set_evt after commit must fail, got %d", after);

    ph_read(2, 1, 1.0);
    ph_dump();
    ph_finalize();

    PH_TEST_PASS(id);
}
