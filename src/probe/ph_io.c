/*
 * =================================================================================
 * PerfHound - Detecting and analyzing performance variation in parallel program
 * 
 * Copyright (C) 2020 Key Liao (Liao Qiucheng)
 * 
 * This program is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software 
 * Foundation, either version 3 of the License, or (at your option) any later 
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with 
 * this program. If not, see https://www.gnu.org/licenses/.
 * 
 * =================================================================================
 * file_io.c
 * Description: File operations for PerfHound.
 * Author: Key Liao
 * Modified: May. 28th, 2020
 * Email: keyliaohpc@gmail.com
 * =================================================================================
 */
//==================================================================================

#define _XOPEN_SOURCE 600

#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "ph_core.h"

/* Variables for files and information */
typedef struct {
    char proj_dir[PATH_MAX]; // Project directory.
    char run_dir[PATH_MAX]; // Records file.
    char host_dir[PATH_MAX]; // Host directory.
    char runinfo[PATH_MAX]; 
    char ctag[PATH_MAX]; // Collected tracing point's descriptions.
    char etag[PATH_MAX]; // Perf event's descriptions.
    char rankmap[PATH_MAX]; // Perf event's descriptions.
    char rec[PATH_MAX]; // Records file.
} ph_path_t;

static int runid;
static uint64_t nrec_tot;
static ph_path_t ph_paths;
static char st_timestr[16], en_timestr[16]; // Timestamp in YYYYMMDDTHHmmss.
/**
 * create data directory for each host. Expending path to absolute
 * data path, save in *hostpath. <Data_Root>/<Proj_Name>/<Host_Name>/<files>.
 * @param path, char*
 * @return int 
 */
int
ph_io_mkdir(char *path){
    int err;
    const char *htmp;
    char tmp_path[PATH_MAX], tmp_path_full[PATH_MAX];
    char *p;

    // walk through path
    strcpy(tmp_path, path);
    memset(tmp_path_full, 0, PATH_MAX * sizeof(char));

    // Expend home directory first if exists.
    if (tmp_path[0] == '~' && tmp_path[1] == '/') {
        htmp = getenv("HOME");
        if(htmp == NULL) {
            htmp = getpwuid(getuid()) -> pw_dir;
        }
        sprintf(tmp_path_full, "%s/%s", htmp, &tmp_path[2]);
        strcpy(tmp_path, tmp_path_full);
    }

    // recursively detect '/' for identifying each path section.
    for (p = tmp_path + 1; *p != '\0'; p++) {
        if (*p == '/') {
            // Jump the first '/' which is root directory.
            *p = '\0';
            printf("*** [PH-Probe] Loop into directory: %s\n", tmp_path);
            err = mkdir(tmp_path, S_IRWXU);
            if (err) {
                if(errno != EEXIST) {
                    printf("*** [PH-Probe] Failed in mkdir, path string: %s\n", tmp_path);
                    fflush(stdout);
                    return errno;
                }
            }
            *p = '/';
        }
    }

    printf("*** [PH-Probe] Loop into directory: %s\n", tmp_path);
    if (mkdir(tmp_path, S_IRWXU) != 0){
        if(errno != EEXIST) {
            printf("*** [PH-Probe] Failed in mkdir, path string: %s\n", tmp_path);
            fflush(stdout);
            return errno;
        }
    }

    return 0;
}





/**
 * Create files. 
 */
int
ph_io_touch(char *path, char *mode){
    FILE *fp;
    
    fp = fopen(path, mode);
    if (fp == NULL)
        return -1;
    fclose(fp);

    return 0;
}

/* Get timestamp of starting point. */
int
ph_io_getstamp(char *path, char *timestr, int *run_id) {
    long int tstamp_size;
    char stampfile[PATH_MAX];
    time_t t = time(0);
    struct tm *lt = localtime(&t);
    FILE *fp;

    sprintf(timestr, "%04d%02d%02dT%02d%02d%02d", lt->tm_year + 1900, 
            lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec); 
    
    /* Create a time stamp remark file to record times of running.*/
    sprintf(stampfile, "%s/tstamp.log", path);
    fp = fopen(stampfile, "a+");
    if(fp == NULL) {
        return -1;
    }
    fseek(fp, 0, SEEK_SET);
    fseek(fp, 0, SEEK_END);
    tstamp_size = ftell(fp);
    *run_id = tstamp_size / 16;
    fclose(fp);

    return 0;
}

/* When varapi ends successfully, put tstamp in tstamp.log in every host directory.*/
void
ph_io_putstamp(char *path, char *timestr) {
    FILE *fp;
    char tstampfile[PATH_MAX];
    
    sprintf(tstampfile, "%s/tstamp.log", path);
    fp = fopen(tstampfile, "a+");
    fseek(fp, 0, SEEK_END);
    fprintf(fp, "%s\n", timestr);
    fclose(fp);
}

/* Logging */
void
ph_io_log(FILE *fp, char *fmt, ...) {
    /* Generate natural timestamp. */
    time_t t = time(0);
    struct tm *lt = localtime(&t);
    char msg[1024];
    va_list args;
    
    // <YYYY><MM><DD>T<hh><mm><ss>,<msg>
    sprintf(msg, "%04d%02d%02dT%02d%02d%02d,", lt->tm_year + 1900, lt->tm_mon + 1, 
            lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
    va_start(args, fmt);
    vsprintf(&msg[16], fmt, args);
    va_end(args);

    fprintf(fp, msg);
    fflush(fp);
    memset(msg, '\0', 1024 * sizeof(char));
}



int
ph_io_mkname(char *root) {
    int nrun;
    char tmp_path[PATH_MAX];
    char *p_realpath = NULL;

    nrec_tot = 0;

    /* Root path. */
    p_realpath = realpath(root, p_realpath);
    if (p_realpath == NULL) {
        return errno;
    }
    strcpy(ph_paths.proj_dir, p_realpath);
    free(p_realpath);

    /* Run path */
    nrun = 0;    
    while (1) {
        nrun += 1;
        sprintf(tmp_path, "%s/run_%d", ph_paths.proj_dir, nrun);
        if( access( tmp_path, F_OK ) != 0 ) {
            break;
        } 
    }
    runid = nrun;
    sprintf(ph_paths.run_dir, "%s/run_%d", ph_paths.proj_dir, runid);

    /* run_info.csv */
    sprintf(ph_paths.runinfo, "%s/run_info.csv", ph_paths.proj_dir); 

    /* Creating ctags.csv. Write headers to ctags.csv */
    sprintf(ph_paths.ctag, "%s/ctag.csv", ph_paths.run_dir);

    /* etag.csv */
    sprintf(ph_paths.etag, "%s/etag.csv", ph_paths.run_dir);
    
    /* rankmap.csv */
    sprintf(ph_paths.rankmap, "%s/rankmap.csv", ph_paths.run_dir);

    /** Considering parallel PerfHound, fill host directory and record file 
     *  without actually creating.
     */
    sprintf(ph_paths.host_dir, "%s/%s", ph_paths.run_dir, ph_pinfo.host);
    sprintf(ph_paths.rec, "%s/r%dc%d.csv", 
        ph_paths.host_dir, ph_pinfo.rank, ph_pinfo.cpu);

    /* Get start timestamp. */
    do{
        time_t t = time(0);
        struct tm *lt = localtime(&t);

        sprintf(st_timestr, "%04d%02d%02dT%02d%02d%02d", lt->tm_year + 1900, 
                lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
    } while(0);

    return 0;
}

int
ph_io_mkfile() {
    int ioerr;
    FILE *fp;

    ioerr = ph_io_mkdir(ph_paths.proj_dir);
    if (ioerr) {
        return ioerr;
    }

    /* Run path. */
    ioerr = mkdir(ph_paths.run_dir, S_IRWXU);
    if (ioerr) {
        return ioerr;
    }

    fp = fopen(ph_paths.runinfo, "a");
    if (fp == NULL) {
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    if (ftell(fp) == 0) {
        // Write headers to an empty file.
        fprintf(fp, "id,nrank,nevent,ngroup,npoint,start_time,end_time,description\n");
    }
    fclose(fp);

    fp = fopen(ph_paths.ctag, "w");
    if (fp == NULL) {
        return -1;
    }
    fprintf(fp, "gid,pid,description\n");
    fclose(fp);

    fp = fopen(ph_paths.etag, "w");
    if (fp == NULL) {
        return -1;
    }
    fprintf(fp, "id,name,code\n");
    fclose(fp);
    fp = fopen(ph_paths.rankmap, "w");
    if (fp == NULL) {
        return -1;
    }
    fprintf(fp, "rank,hostname,cpu,hosthead,iorank\n");
    fclose(fp);

    return 0;
}

/**
 * Wrapper for creating host directory.
 * @return int 
 */
int
ph_io_mkhost() {
    int ioerr = 0;

    if (access(ph_paths.host_dir, F_OK) != 0) {
        ioerr = mkdir(ph_paths.host_dir, S_IRWXU);
        if (ioerr) {
            if(errno != EEXIST) {
                printf("*** [PH-Probe] Failed in mkdir, path string: %s\n", ph_paths.host_dir);
                fflush(stdout);
                return errno;
            }
        }
    }

    return ioerr;
}

int
ph_io_mkrec() {
    FILE *fp = NULL;
    
    // NULL for single-process run, import the record file address for
    // processing IO for multiple processes.
    fp = fopen(ph_paths.rec, "w");
    if (fp == NULL) {
        return errno;
    }

    fprintf(fp, "gid,pid,cycle,nanosec,uval");
    for (int i = 1; i <= ph_nev; i ++) {
        fprintf(fp, ",ev%d", i);
    }
    fprintf(fp, "\n");


    fclose(fp);

    return 0;
}

int
ph_io_wtctag(uint32_t gid, uint32_t pid, const char *tagstr) {
    FILE *fp = fopen(ph_paths.ctag, "a");

    fprintf(fp, "%u,%u,%s\n", gid, pid, tagstr);

    fflush(fp);
    fclose(fp);

    return 0;
}

/**
 * @brief 
 * @param id 
 * @param evtstr 
 * @param evcode 
 * @return int 
 */
int
ph_io_wtetag(int id, const char *evtstr, uint64_t evcode) {
    FILE *fp = fopen(ph_paths.etag, "a");

    fprintf(fp, "%d,%s,0x%llx\n", id, evtstr, evcode);

    fflush(fp);
    fclose(fp);

    return 0;
}


int ph_io_wtrankmap() {
    FILE *fp;
    fp = fopen(ph_paths.rankmap, "a");

    fprintf(fp, "%d,%s,%d,%d,%d\n", ph_pinfo.rank, ph_pinfo.host, ph_pinfo.cpu, ph_pinfo.head, ph_pinfo.iorank);

    fflush(fp);
    fclose(fp);

    return 0;
}


int ph_io_wtinfo() {
    // id, mode, nrank, nrec, start_time, end_time, description, 
    /* Get end timestamp. */
    do{
        time_t t = time(0);
        struct tm *lt = localtime(&t);

        sprintf(en_timestr, "%04d%02d%02dT%02d%02d%02d", lt->tm_year + 1900, 
                lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
    } while(0);

    FILE *fp = fopen(ph_paths.runinfo, "a");

    if (fp == NULL) {
        return 1;
    }
    fprintf(fp, "%d,%d,%d,%d,%s,%s,\n",
        runid, ph_pinfo.nrank, ph_nev, nrec_tot, st_timestr, en_timestr);

    fflush(fp);
    fclose(fp);
    return 0;
}

/**
 * @brief 
 */
int
ph_io_wtrec(int nrec) {
    FILE *p_recfile;

    nrec_tot += nrec;

    p_recfile = fopen(ph_paths.rec, "a");

    if (p_recfile == NULL) {
        return 1;
    }
#ifdef PH_OPT_PAPI

    for (int i = 0; i < nrec; i ++) {
        fprintf(p_recfile, "%u,%u,%lld,%lld,%f", 
                ph_precs[i].ctag[0], ph_precs[i].ctag[1], 
                ph_precs[i].cy, ph_precs[i].ns, ph_precs[i].uval);
        for (int j = 0; j < ph_nev; j++) {
            fprintf(p_recfile, ",%lld", ph_precs[i].ev[j]);
        }
        fprintf(p_recfile, "\n");
    }
#else
    for (int i = 0; i < nrec; i ++) {
        fprintf(p_recfile, "%u,%u,%llu,%llu,%f", 
                ph_precs[i].ctag[0], ph_precs[i].ctag[1], 
                ph_precs[i].cy, ph_precs[i].ns, ph_precs[i].uval);
        for (int j = 0; j < ph_nev; j++) {
            fprintf(p_recfile, ",%llu", ph_precs[i].ev[j]);
        }
        fprintf(p_recfile, "\n");
    }
#endif

    fflush(p_recfile);
    fclose(p_recfile);

    return 0;
}