/**
 * AM1 — MPI init path: MPI_Init, phmpi_init, commit, finalize (2 ranks).
 */
#include <mpi.h>
#include "ph_mpi.h"
#include "../common/ph_test.h"

#include <string.h>

int
main(int argc, char **argv)
{
    const char *id = "AM1";
    char tmpdir[256];
    int rank;
    int err;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                        "failed to create temp directory");
    }
    MPI_Bcast(tmpdir, 256, MPI_CHAR, 0, MPI_COMM_WORLD);

    err = phmpi_init(tmpdir);
    PH_TEST_REQUIRE(id, err == 0, "rank %d phmpi_init failed", rank);

    phmpi_commit();
    phmpi_read(1, 1, (double)rank);
    phmpi_finalize();

    MPI_Finalize();
    if (rank == 0) {
        PH_TEST_PASS(id);
    }
    return 0;
}
