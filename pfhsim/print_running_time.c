#include "print_running_time.h"

void
out_put(double **record_time, char **record_name, int record_count, const int run_i){
    //write to file
    FILE *file;
    char filename[20]={0};
    sprintf(filename, "out/run%d", run_i);
    file = fopen(filename,  "w");
    char **str = (char**) malloc(sizeof(char*) * total_core_num);
    for (int i = 0; i < total_core_num; i ++){
        str[i] = (char*) malloc(sizeof(char) * record_count * 40);
        sprintf(str[i], "core%d: ", i);
        for (int j = 0; j < record_count; j++){
            strcat(str[i], record_name[j]);
            strcat(str[i], ":");
            char *runtime_str = (char*) malloc(sizeof(char) * 20);
            sprintf(runtime_str, "%.3lf", record_time[j][i]);
            strcat(str[i], runtime_str);
            strcat(str[i], " seconds    ");
        }
        strcat(str[i], "\n");
        fprintf(file, "%s", str[i]);
        free(str[i]);
    }
    fclose(file);
    free(str);
}

void
print_running_time(graph **m_graph, int run_i){
    int record_count = 0;
    char **record_name;
    double **local_record_time;
    double **global_record_time;
    //get the number of functions that need to be printed
    for (int i = 0; i < m_graph[0]->node_num; i ++){
        if(m_graph[0]->node[i]->record_flag == 1){
            record_count ++;
        }
    }
    record_name = (char**) malloc(sizeof(char*) * record_count);
    global_record_time = (double**) malloc(sizeof(double*) * record_count);
    local_record_time = (double**) malloc(sizeof(double*) * record_count);
    for (int i = 0; i < record_count; i ++){
        if(my_rank == 0){
            record_name[i] = (char*) malloc(sizeof(char) * MAX_NAME_SIZE);
            global_record_time[i] = (double*) malloc(sizeof(double) * total_core_num);
        }else{
            record_name[i] = NULL;
            global_record_time[i] = NULL;
        }
        local_record_time[i] = (double*) malloc(sizeof(double) * local_sz);
    }

    //get functions' name that need to be printed
    if(my_rank == 0){
        int j = 0;
        for (int i = 0; i < m_graph[0]->node_num; i ++){
            if(m_graph[0]->node[i]->record_flag == 1){
                strcpy(record_name[j], m_graph[0]->node[i]->name);
                j ++;
            }
        }    
    }

    //get functions' running time that need to be printed
    int k = 0;
    for (int i = 0; i < m_graph[0]->node_num; i ++){
        if(m_graph[0]->node[i]->record_flag == 1){
            for (int j = 0; j < local_sz; j++){
                local_record_time[k][j] = m_graph[j]->node[i]->total_mrunning_time;  
            }
            //send to root process
            MPI_Gatherv(local_record_time[k], local_sz, MPI_DOUBLE, global_record_time[k],
             all_local_sz, all_displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            k ++;
        }
    }
    if(my_rank == 0 && record_count > 0){
        out_put(global_record_time, record_name, record_count, run_i);
    }
    //clean
    for (int i = 0; i < record_count; i ++){
        if(local_record_time[i] != NULL){
            free(local_record_time[i]);
        }
        if(record_name[i] != NULL){
            free(record_name[i]);
        }
        if(global_record_time[i] != NULL){
            free(global_record_time[i]);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

