/**
 * AM4 — MPI fastread: phmpi_fastread samples without auto-dump until finalize.
 */
#include <mpi.h>
#include "ph_mpi.h"
#include "../common/ph_test.h"

#include <string.h>

int
main(int argc, char **argv)
{
    const char *id = "AM4";
    char tmpdir[256];
    char run_path[512];
    char rec_path[512];
    int rank;
    int i;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                        "failed to create temp directory");
    }
    MPI_Bcast(tmpdir, 256, MPI_CHAR, 0, MPI_COMM_WORLD);

    PH_TEST_REQUIRE(id, phmpi_init(tmpdir) == 0, "phmpi_init failed");
    phmpi_commit();

    for (i = 0; i < 3; i++) {
        phmpi_fastread(3, 1, (double)i);
        ph_test_busy_work(1000);
    }
    phmpi_finalize();

    if (rank == 0) {
        PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                    sizeof(run_path)) == 0,
                        "run directory missing");
        PH_TEST_REQUIRE(id, ph_test_find_rank_rec(run_path, 0, rec_path,
                                                  sizeof(rec_path)) == 0,
                        "record file missing");
        PH_TEST_REQUIRE(id, ph_test_rec_has_tag(rec_path, 3, 1),
                        "fastread samples missing in record");
    }

    MPI_Finalize();
    if (rank == 0) {
        PH_TEST_PASS(id);
    }
    return 0;
}
