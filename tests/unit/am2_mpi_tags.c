/**
 * AM2 — MPI tags barrier: all ranks call phmpi_set_tag, verify ctag.csv.
 */
#include <mpi.h>
#include "ph_mpi.h"
#include "../common/ph_test.h"

#include <stdio.h>
#include <string.h>

int
main(int argc, char **argv)
{
    const char *id = "AM2";
    char tmpdir[256];
    char run_path[512];
    char ctag_path[512];
    char tagbuf[64];
    int rank;
    int nprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (rank == 0) {
        PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                        "failed to create temp directory");
    }
    MPI_Bcast(tmpdir, 256, MPI_CHAR, 0, MPI_COMM_WORLD);

    PH_TEST_REQUIRE(id, phmpi_init(tmpdir) == 0, "phmpi_init failed on rank %d",
                    rank);

    snprintf(tagbuf, sizeof(tagbuf), "RankTag_%d", rank);
    PH_TEST_REQUIRE(id, phmpi_set_tag(5, (uint32_t)rank, tagbuf) == 0,
                    "phmpi_set_tag failed on rank %d", rank);

    phmpi_commit();
    phmpi_read(5, (uint32_t)rank, (double)rank);
    phmpi_finalize();

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                    sizeof(run_path)) == 0,
                        "run directory missing");
        snprintf(ctag_path, sizeof(ctag_path), "%s/ctag.csv", run_path);
        PH_TEST_REQUIRE(id, ph_test_ctag_has(ctag_path, 5, 0, "RankTag_0"),
                        "ctag missing rank 0 tag");
        PH_TEST_REQUIRE(id, nprocs >= 2, "expected at least 2 MPI ranks");
    }

    MPI_Finalize();
    if (rank == 0) {
        PH_TEST_PASS(id);
    }
    return 0;
}
