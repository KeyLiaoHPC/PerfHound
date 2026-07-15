/**
 * Lightweight helpers for PerfHound functional and integration tests.
 * Exit codes: 0 = PASS, 2 = SKIP (environment), non-zero otherwise = FAIL.
 */
#ifndef PH_TEST_H
#define PH_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PH_TEST_EXIT_PASS 0
#define PH_TEST_EXIT_SKIP 2
#define PH_TEST_EXIT_FAIL 1

#define PH_TEST_FAIL(_id, _fmt, ...)                                        \
    do {                                                                    \
        fprintf(stderr, "*** [PH-tests] [%s] FAIL: " _fmt "\n", (_id),     \
                ##__VA_ARGS__);                                             \
        return PH_TEST_EXIT_FAIL;                                           \
    } while (0)

#define PH_TEST_SKIP(_id, _fmt, ...)                                        \
    do {                                                                    \
        fprintf(stderr, "*** [PH-tests] [%s] SKIP: " _fmt "\n", (_id),     \
                ##__VA_ARGS__);                                             \
        return PH_TEST_EXIT_SKIP;                                           \
    } while (0)

#define PH_TEST_REQUIRE(_id, _cond, _fmt, ...)                               \
    do {                                                                    \
        if (!(_cond)) {                                                     \
            PH_TEST_FAIL((_id), _fmt, ##__VA_ARGS__);                       \
        }                                                                   \
    } while (0)

#define PH_TEST_PASS(_id)                                                   \
    do {                                                                    \
        printf("*** [PH-tests] [%s] PASS\n", (_id));                        \
        return PH_TEST_EXIT_PASS;                                           \
    } while (0)

/* Shared CSV / filesystem helpers (implemented in ph_test_util.c). */
int ph_test_mktemp_dir(char *buf, size_t len);
int ph_test_find_latest_run(const char *root, char *run_path, size_t len);
int ph_test_path_exists(const char *path);
long ph_test_file_lines(const char *path);
int ph_test_ctag_has(const char *ctag_path, unsigned gid, unsigned pid,
                     const char *desc_substr);
int ph_test_rec_has_tag(const char *rec_path, unsigned gid, unsigned pid);
int ph_test_rec_count_user_samples(const char *rec_path, unsigned gid,
                                   int *out_count);
int ph_test_rec_monotonic_times(const char *rec_path, int *out_rows);
int ph_test_rec_delta_for_tag(const char *rec_path, unsigned gid,
                              unsigned pid, long long *out_dcy,
                              long long *out_dns);
int ph_test_rec_uval_at_tag(const char *rec_path, unsigned gid,
                            unsigned pid, double *out_uval);
int ph_test_find_rec_file(const char *run_path, char *rec_path, size_t len);
int ph_test_etag_nonempty(const char *run_path);
int ph_test_rankmap_rows(const char *run_path, int min_rows);
int ph_test_find_rank_rec(const char *run_path, int rank, char *rec_path,
                          size_t len);
int ph_test_rec_first_ev_column(const char *rec_path, long long *out_ev);
int ph_test_rec_ev_delta(const char *rec_path, long long *out_delta);
void ph_test_busy_work(unsigned long iters);
int ph_test_rec_has_ev_columns(const char *rec_path);

#endif /* PH_TEST_H */
