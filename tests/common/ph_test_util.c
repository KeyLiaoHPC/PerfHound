/**
 * CSV and filesystem utilities for PerfHound test harness.
 */
#define _GNU_SOURCE

#include "ph_test.h"

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int
ph_test_mktemp_dir(char *buf, size_t len)
{
    if (len < 16) {
        return -1;
    }
    snprintf(buf, len, "/tmp/ph_test_XXXXXX");
    if (mkdtemp(buf) == NULL) {
        return -1;
    }
    return 0;
}

int
ph_test_path_exists(const char *path)
{
    struct stat st;

    return stat(path, &st) == 0;
}

int
ph_test_find_latest_run(const char *root, char *run_path, size_t len)
{
    DIR *dir;
    struct dirent *ent;
    int best = 0;

    dir = opendir(root);
    if (dir == NULL) {
        return -1;
    }

    while ((ent = readdir(dir)) != NULL) {
        int id;

        if (sscanf(ent->d_name, "run_%d", &id) == 1 && id > best) {
            best = id;
        }
    }
    closedir(dir);

    if (best <= 0) {
        return -1;
    }

    snprintf(run_path, len, "%s/run_%d", root, best);
    return 0;
}

long
ph_test_file_lines(const char *path)
{
    FILE *fp;
    long lines = 0;
    int ch;
    int has_char = 0;

    fp = fopen(path, "r");
    if (fp == NULL) {
        return -1;
    }

    while ((ch = fgetc(fp)) != EOF) {
        has_char = 1;
        if (ch == '\n') {
            lines++;
        }
    }
    fclose(fp);

    if (has_char && lines == 0) {
        lines = 1;
    }

    return lines;
}

int
ph_test_ctag_has(const char *ctag_path, unsigned gid, unsigned pid,
                 const char *desc_substr)
{
    FILE *fp;
    char line[512];
    unsigned g;
    unsigned p;

    fp = fopen(ctag_path, "r");
    if (fp == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char desc[256];

        if (sscanf(line, "%u,%u,%255[^\n]", &g, &p, desc) != 3) {
            continue;
        }
        if (g == gid && p == pid) {
            if (desc_substr == NULL || strstr(desc, desc_substr) != NULL) {
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);
    return 0;
}

static int
_sf_parse_rec_line(const char *line, unsigned *gid, unsigned *pid,
                   long long *cy, long long *ns, double *uval)
{
    return sscanf(line, "%u,%u,%lld,%lld,%lf", gid, pid, cy, ns, uval) == 5;
}

int
ph_test_rec_has_tag(const char *rec_path, unsigned gid, unsigned pid)
{
    FILE *fp;
    char line[1024];
    unsigned g;
    unsigned p;
    long long cy;
    long long ns;
    double uval;

    fp = fopen(rec_path, "r");
    if (fp == NULL) {
        return 0;
    }

    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (_sf_parse_rec_line(line, &g, &p, &cy, &ns, &uval) && g == gid
            && p == pid) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int
ph_test_rec_count_user_samples(const char *rec_path, unsigned gid,
                               int *out_count)
{
    FILE *fp;
    char line[1024];
    unsigned g;
    unsigned p;
    long long cy;
    long long ns;
    double uval;
    int count = 0;

    fp = fopen(rec_path, "r");
    if (fp == NULL) {
        return -1;
    }

    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (_sf_parse_rec_line(line, &g, &p, &cy, &ns, &uval) && g == gid) {
            count++;
        }
    }

    fclose(fp);
    *out_count = count;
    return 0;
}

int
ph_test_rec_monotonic_times(const char *rec_path, int *out_rows)
{
    FILE *fp;
    char line[1024];
    unsigned g;
    unsigned p;
    long long cy;
    long long ns;
    double uval;
    long long prev_cy = 0;
    long long prev_ns = 0;
    int rows = 0;
    int all_zero = 1;

    fp = fopen(rec_path, "r");
    if (fp == NULL) {
        return -1;
    }

    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (!_sf_parse_rec_line(line, &g, &p, &cy, &ns, &uval)) {
            continue;
        }
        rows++;
        if (cy != 0 || ns != 0) {
            all_zero = 0;
        }
        if (rows > 1 && (cy < prev_cy || ns < prev_ns)) {
            fclose(fp);
            return -2;
        }
        prev_cy = cy;
        prev_ns = ns;
    }

    fclose(fp);
    if (rows == 0 || all_zero) {
        return -3;
    }

    *out_rows = rows;
    return 0;
}

static int
_sf_find_tag_times(const char *rec_path, unsigned gid, unsigned pid,
                   long long *cy, long long *ns)
{
    FILE *fp;
    char line[1024];
    unsigned g;
    unsigned p;
    long long c;
    long long n;
    double uval;

    fp = fopen(rec_path, "r");
    if (fp == NULL) {
        return -1;
    }

    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (_sf_parse_rec_line(line, &g, &p, &c, &n, &uval) && g == gid
            && p == pid) {
            *cy = c;
            *ns = n;
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return -1;
}

int
ph_test_rec_delta_for_tag(const char *rec_path, unsigned gid,
                          unsigned pid_start, long long *out_dcy,
                          long long *out_dns)
{
    long long cy0;
    long long cy1;
    long long ns0;
    long long ns1;

    if (_sf_find_tag_times(rec_path, gid, pid_start, &cy0, &ns0) != 0) {
        return -1;
    }
    if (_sf_find_tag_times(rec_path, gid, pid_start + 1, &cy1, &ns1) != 0) {
        return -1;
    }

    *out_dcy = cy1 - cy0;
    *out_dns = ns1 - ns0;
    return 0;
}

int
ph_test_rec_uval_at_tag(const char *rec_path, unsigned gid, unsigned pid,
                        double *out_uval)
{
    FILE *fp;
    char line[1024];
    unsigned g;
    unsigned p;
    long long cy;
    long long ns;
    double uval;

    fp = fopen(rec_path, "r");
    if (fp == NULL) {
        return -1;
    }

    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (_sf_parse_rec_line(line, &g, &p, &cy, &ns, &uval) && g == gid
            && p == pid) {
            *out_uval = uval;
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return -1;
}

int
ph_test_find_rec_file(const char *run_path, char *rec_path, size_t len)
{
    char host_path[PATH_MAX];
    DIR *dir;
    struct dirent *ent;

    dir = opendir(run_path);
    if (dir == NULL) {
        return -1;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.') {
            continue;
        }
        snprintf(host_path, sizeof(host_path), "%s/%s", run_path, ent->d_name);
        if (!ph_test_path_exists(host_path)) {
            continue;
        }

        DIR *hdir = opendir(host_path);
        struct dirent *hent;

        if (hdir == NULL) {
            continue;
        }

        while ((hent = readdir(hdir)) != NULL) {
            if (strncmp(hent->d_name, "r0c", 3) == 0
                && strstr(hent->d_name, ".csv") != NULL) {
                snprintf(rec_path, len, "%s/%s", host_path, hent->d_name);
                closedir(hdir);
                closedir(dir);
                return 0;
            }
        }
        closedir(hdir);
    }

    closedir(dir);
    return -1;
}

int
ph_test_etag_nonempty(const char *run_path)
{
    char path[PATH_MAX];
    long lines;

    snprintf(path, sizeof(path), "%s/etag.csv", run_path);
    lines = ph_test_file_lines(path);
    return lines > 1;
}

int
ph_test_rankmap_rows(const char *run_path, int min_rows)
{
    char path[PATH_MAX];
    long lines;

    snprintf(path, sizeof(path), "%s/rankmap.csv", run_path);
    lines = ph_test_file_lines(path);
    return lines >= (min_rows + 1);
}

int
ph_test_find_rank_rec(const char *run_path, int rank, char *rec_path,
                      size_t len)
{
    char host_path[PATH_MAX];
    DIR *dir;
    struct dirent *ent;
    char pattern[32];

    snprintf(pattern, sizeof(pattern), "r%dc", rank);

    dir = opendir(run_path);
    if (dir == NULL) {
        return -1;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.') {
            continue;
        }
        snprintf(host_path, sizeof(host_path), "%s/%s", run_path, ent->d_name);

        DIR *hdir = opendir(host_path);
        struct dirent *hent;

        if (hdir == NULL) {
            continue;
        }

        while ((hent = readdir(hdir)) != NULL) {
            if (strncmp(hent->d_name, pattern, strlen(pattern)) == 0
                && strstr(hent->d_name, ".csv") != NULL) {
                snprintf(rec_path, len, "%s/%s", host_path, hent->d_name);
                closedir(hdir);
                closedir(dir);
                return 0;
            }
        }
        closedir(hdir);
    }

    closedir(dir);
    return -1;
}

int
ph_test_rec_first_ev_column(const char *rec_path, long long *out_ev)
{
    FILE *fp;
    char line[1024];
    unsigned gid;
    unsigned pid;
    long long cy;
    long long ns;
    double uval;
    long long ev;

    fp = fopen(rec_path, "r");
    if (fp == NULL) {
        return -1;
    }

    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "%u,%u,%lld,%lld,%lf,%lld", &gid, &pid, &cy, &ns,
                   &uval, &ev) == 6) {
            *out_ev = ev;
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return -1;
}

int
ph_test_rec_ev_delta(const char *rec_path, long long *out_delta)
{
    FILE *fp;
    char line[1024];
    unsigned gid;
    unsigned pid;
    long long cy;
    long long ns;
    double uval;
    long long first = -1;
    long long last = -1;

    fp = fopen(rec_path, "r");
    if (fp == NULL) {
        return -1;
    }

    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        long long ev;

        if (sscanf(line, "%u,%u,%lld,%lld,%lf,%lld", &gid, &pid, &cy, &ns,
                   &uval, &ev) == 6) {
            if (first < 0) {
                first = ev;
            }
            last = ev;
        }
    }

    fclose(fp);
    if (first < 0 || last < 0) {
        return -1;
    }

    *out_delta = last - first;
    return 0;
}

void
ph_test_busy_work(unsigned long iters)
{
    volatile double x = 1.0;
    unsigned long i;

    for (i = 0; i < iters; i++) {
        x += (double)(i & 0xff) * 0.0001;
    }
    (void)x;
}

int
ph_test_rec_has_ev_columns(const char *rec_path)
{
    FILE *fp;
    char line[256];

    fp = fopen(rec_path, "r");
    if (fp == NULL) {
        return 0;
    }

    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }
    fclose(fp);

    return strstr(line, "ev1") != NULL;
}
