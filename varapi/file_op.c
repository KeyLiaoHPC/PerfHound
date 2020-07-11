/*
 * =================================================================================
 * VarTect - Detecting and analyzing performance variation in parallel program
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
 * file_op.c
 * Description: File operations for Vartect.
 * Author: Key Liao
 * Modified: May. 28th, 2020
 * Email: keyliaohpc@gmail.com
 * =================================================================================
 */
//==================================================================================

/* Comment Syntax */
/*
 * Long block document.
 */
/* Beginning remark for functioning block/procedure. */
// Short comment for variables, short explanation.

//==================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "varapi_core.h"

/**
 * @brief create data directory for each host. Expending path to absolute
 * data path, save in *hostpath. <Data_Root>/<Proj_Name>/<Host_Name>/<files>.
 * @param u_data_root   User-input root data directory.
 * @param u_proj_name   User-input project name.
 * @param hostname      Hostname in char*.
 * @param hostpath      Absolute path to be generated for the host.
 * @return int 
 */
int
vt_mkdir(char *path){
    int err;
    char tmp_path[_PATH_MAX];
    char *p;

    // walk through path
    strcpy(tmp_path, path);
    memset(path, 0, _PATH_MAX * sizeof(char));

    for (p = tmp_path; *p != '\0'; p++) {
        if (*p == '/') {
            *p = '\0';
            err = mkdir(tmp_path, S_IRWXU);
            if(err != 0) {
                if(errno != EEXIST)
                    return -1;
            }
            *p = '/';
        }
    }

    if (mkdir(tmp_path, S_IRWXU) != 0){
        if(errno != EEXIST)
            return -1;
    }

    // expand relative path into absolute path to avoid runtime path changing 
    realpath(tmp_path, path);
    
    return 0;
}

/* Create files. */
int
vt_touch(char *path, char *mode){
    FILE *fp;
    
    fp = fopen(path, mode);
    if (fp == NULL)
        return -1;
    fclose(fp);

    return 0;
}

/* Get timestamp of starting point. */
int
vt_getstamp(char *path, char *timestr, int *run_id) {
    long int tstamp_size;
    char stampfile[_PATH_MAX];
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
vt_putstamp(char *path, char *timestr) {
    FILE *fp;
    char tstampfile[_PATH_MAX];
    
    sprintf(tstampfile, "%s/tstamp.log", path);
    fp = fopen(tstampfile, "a+");
    fseek(fp, 0, SEEK_END);
    fprintf(fp, "%s\n", timestr);
    fclose(fp);
}

/* Logging */
void
vt_log(FILE *fp, char *fmt, ...) {
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
    memset(msg, '\0', 1024 * sizeof(char));
}
