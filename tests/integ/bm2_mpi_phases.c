/**
 * BM2 — Rank-local phases: per-rank tags and rankmap consistency.
 */
#include <mpi.h>
#include "ph_mpi.h"
#include "../common/ph_test.h"

#include <stdio.h>
#include <string.h>

int
main(int argc, char **argv)
{
    const char *id = "BM2";
    char tmpdir[256];
    char run_path[512];
    char ctag_path[512];
    char rec_path[512];
    char phase[64];
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

    PH_TEST_REQUIRE(id, phmpi_init(tmpdir) == 0, "phmpi_init failed");

    snprintf(phase, sizeof(phase), "RankPhase_%d", rank);
    PH_TEST_REQUIRE(id, phmpi_set_tag(50, (uint32_t)rank, phase) == 0,
                    "set_tag failed on rank %d", rank);

    phmpi_commit();
    phmpi_read(50, (uint32_t)rank, (double)rank);
    phmpi_finalize();

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                    sizeof(run_path)) == 0,
                        "run directory missing");
        PH_TEST_REQUIRE(id, ph_test_rankmap_rows(run_path, nprocs),
                        "rankmap incomplete");

        snprintf(ctag_path, sizeof(ctag_path), "%s/ctag.csv", run_path);
        PH_TEST_REQUIRE(id, ph_test_ctag_has(ctag_path, 50, 0, "RankPhase_0"),
                        "ctag missing rank 0 phase tag");

        PH_TEST_REQUIRE(id, ph_test_find_rank_rec(run_path, 1, rec_path,
                                                  sizeof(rec_path)) == 0,
                        "rank 1 record missing");
        PH_TEST_REQUIRE(id, ph_test_rec_has_tag(rec_path, 50, 1),
                        "rank 1 sample tag missing");
    }

    MPI_Finalize();
    if (rank == 0) {
        PH_TEST_PASS(id);
    }
    return 0;
}
