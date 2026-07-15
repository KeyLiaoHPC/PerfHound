/**
 * AM3 — MPI per-rank IO: each rank produces r<rank>c*.csv and rankmap rows.
 */
#include <mpi.h>
#include "ph_mpi.h"
#include "../common/ph_test.h"

#include <string.h>

int
main(int argc, char **argv)
{
    const char *id = "AM3";
    char tmpdir[256];
    char run_path[512];
    char rec_path[512];
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
    phmpi_commit();
    phmpi_read(1, 1, (double)rank);
    phmpi_dump();
    phmpi_finalize();

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        PH_TEST_REQUIRE(id, ph_test_find_latest_run(tmpdir, run_path,
                                                    sizeof(run_path)) == 0,
                        "run directory missing");
        PH_TEST_REQUIRE(id, ph_test_rankmap_rows(run_path, nprocs),
                        "rankmap.csv has fewer than %d data rows", nprocs);
        PH_TEST_REQUIRE(id, ph_test_find_rank_rec(run_path, 0, rec_path,
                                                  sizeof(rec_path)) == 0,
                        "missing rank 0 record file");
        PH_TEST_REQUIRE(id, ph_test_find_rank_rec(run_path, 1, rec_path,
                                                  sizeof(rec_path)) == 0,
                        "missing rank 1 record file");
    }

    MPI_Finalize();
    if (rank == 0) {
        PH_TEST_PASS(id);
    }
    return 0;
}
