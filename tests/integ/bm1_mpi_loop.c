/**
 * BM1 — Dual-rank instrumented loop: each rank runs busy loop with phmpi_read pairs.
 */
#include <mpi.h>
#include "ph_mpi.h"
#include "../common/ph_test.h"

#include <string.h>

int
main(int argc, char **argv)
{
    const char *id = "BM1";
    char tmpdir[256];
    char run_path[512];
    char rec_path[512];
    long long dcy;
    long long dns;
    int rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        PH_TEST_REQUIRE(id, ph_test_mktemp_dir(tmpdir, sizeof(tmpdir)) == 0,
                        "failed to create temp directory");
    }
    MPI_Bcast(tmpdir, 256, MPI_CHAR, 0, MPI_COMM_WORLD);

    PH_TEST_REQUIRE(id, phmpi_init(tmpdir) == 0, "phmpi_init failed");
    PH_TEST_REQUIRE(id, phmpi_set_tag(40, 1, "MpiLoopStart") == 0, "set_tag failed");

    phmpi_commit();
    phmpi_read(40, 1, (double)rank);
    ph_test_busy_work(300000);
    phmpi_read(40, 2, (double)rank);
    phmpi_finalize();

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                    sizeof(run_path)) == 0,
                        "run directory missing");

        PH_TEST_REQUIRE(id, ph_test_find_rank_rec(run_path, 0, rec_path,
                                                  sizeof(rec_path)) == 0,
                        "rank 0 record missing");
        PH_TEST_REQUIRE(id, ph_test_rec_delta_for_tag(rec_path, 40, 1, &dcy, &dns) == 0
                        && dns > 0,
                        "rank 0 timing delta unreasonable (dns=%lld dcy=%lld)",
                        dns, dcy);

        PH_TEST_REQUIRE(id, ph_test_find_rank_rec(run_path, 1, rec_path,
                                                  sizeof(rec_path)) == 0,
                        "rank 1 record missing");
        PH_TEST_REQUIRE(id, ph_test_rec_delta_for_tag(rec_path, 40, 1, &dcy, &dns) == 0
                        && dns > 0,
                        "rank 1 timing delta unreasonable (dns=%lld dcy=%lld)",
                        dns, dcy);
    }

    MPI_Finalize();
    if (rank == 0) {
        PH_TEST_PASS(id);
    }
    return 0;
}
