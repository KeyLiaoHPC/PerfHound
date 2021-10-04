#include "frame_init.h"
#include "print_running_time.h"

//node configration
int node_num;
int core_num;
noise *node_noise;
int total_core_num;
//os noise configration
int8_t os_switch;
double os_period;
double os_detour;
double last_sync_time = 0;
double *node_osbias;
//gsl 
const gsl_rng_type *T;
gsl_rng **r;
uint64_t *seed;
//mpi
int my_rank;
int comm_sz;
int local_start;
int local_sz;
int local_node_start;
int local_node_sz;
int *all_local_sz;
int *all_displs;
int *all_node_sz;
int *all_node_displs;
int num_run;


double 
__get_std_time(graph_node *node){
	double time = 0;
	time += node->run_time;
	for (int i = 0; i < node->child_num; i ++){
		time += __get_std_time(node->childs[i]) * node->loop_nums[i];
	}
	return time;
}

double 
get_std_time(graph *m_graph){
    if(my_rank == 0){
        graph_node *main = m_graph->root_node;
        double time = 0;
        time = __get_std_time(main);
        printf("Theoretical Time: %lf seconds\n", time);
    }
}

void 
Init(int argc, char **argv, graph ***m_graph){
    if (argc == 2){
        num_run = atoi(argv[1]);
    }else{
        num_run = NUM_RUN_DEFAULT;
    }
    parse_node_config("./config/node.json");
    parse_os_config("./config/os.json");   
    //distribute sim core
    total_core_num = core_num * node_num;
    distribute_job();
    //parse Program
    parse_program(m_graph, "./config/program.json"); 
    if(my_rank == 0){
        printf("Nodes: %d, Cores: %d\n", node_num, core_num);
    }
}

int 
get_urandom(uint64_t *x) {
    FILE *fp = fopen("/dev/urandom", "r");
    if (fp == NULL) {
        printf("Failed to open random file.\n");
        return 1;
    } 

    fread(x, sizeof(uint64_t), 1, fp); 

    fclose(fp);

    return 0;
}

/**
 * 
 * Node performance variation
 * @param pnode  pointers to node var perf factor
 * @param n      # of nodes.
 * @return int 
 */
int
get_nodevar(double *pnode, int n) {
    if (n == 0) {
        return 0;
    }
    int i;
    double sigma, ucut, dcut;
    uint64_t mseed;

    gsl_rng_env_setup();
    gsl_rng *mr;
    const gsl_rng_type *mT;
    mT = gsl_rng_ranlux389;
    mr = gsl_rng_alloc(mT);
    get_urandom(&mseed);
    gsl_rng_set(mr, mseed);


    sigma = 3 * node_noise->parameters[0]; // 3sigma for +-5% of 99.9% ndoes.
    ucut = 0 + node_noise->parameters[1];  // Cutoff at +NORM_CUT
    dcut = 0 - node_noise->parameters[1];  // Cutoff at -NORM_CUT

    for (i = 0; i < n; i ++) {
        pnode[i] = gsl_ran_gaussian(mr, sigma);
        pnode[i] = pnode[i] > ucut? ucut: pnode[i];
        pnode[i] = pnode[i] < dcut? dcut: pnode[i];
        pnode[i] += 1;
    } 
    gsl_rng_free(mr);

    return 0;
}

/*
* add node noise.
*/
void
add_node_noise(graph **m_graph){
    if(node_noise == NULL) //no node noise
        return;

    double *node_perf = (double*) malloc(sizeof(double) * node_num);
    if(my_rank == 0)
        get_nodevar(node_perf, node_num);
    MPI_Bcast(node_perf, node_num, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_sz; i ++){
        int sim_node = (i + local_start) / core_num;
        for (int j = 0; j < m_graph[i]->node_num ; j ++){
            m_graph[i]->node[j]->run_time = m_graph[i]->node[j]->std_run_time / node_perf[sim_node];  
        }
    }
    free(node_perf);
}

double
add_noise(graph_node *node, int i){
    if(node->m_noise == NULL || node->run_time == 0) //no noise
        return node->run_time;

    if(strcmp(node->m_noise->noise_type, "pareto") == 0){//pareto distribution
        double s = gsl_ran_pareto(r[0], node->m_noise->parameters[0], 1);
        s = s > 2? 2: s;
        return(node->run_time * s);
    }
    return node->run_time; 
}

void
update_osbias(double *node_osbias, int nnode, int p, double passed_time) {
    int passed_num;
    for (int i = 0; i < nnode; i ++) {
        passed_num = (node_osbias[i] + passed_time) / p; 
        node_osbias[i] = node_osbias[i] + passed_time - passed_num * p;
    }
}

double
add_os_noise(double *last_sync_time, double *local_total_run_time){
    uint64_t rand;
    double node_off,tmax,local_final = 0,global_final = 0;
    int nnoise,passed_noise,core,i,j,k;
    double *ptr;

    //gather local run time
    double *global_total_run_time;
    if(my_rank == 0){
        global_total_run_time = (double*) malloc(sizeof(double) * core_num * node_num);
    }
    MPI_Gatherv(local_total_run_time, local_sz, MPI_DOUBLE, global_total_run_time, 
     all_local_sz, all_displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //distribute job
    double* local_ptime = (double*) malloc(sizeof(double) * local_node_sz * core_num);
    MPI_Scatterv(global_total_run_time, all_node_sz, all_node_displs, MPI_DOUBLE,
     local_ptime, local_node_sz * core_num, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (i = 0; i < local_node_sz; i ++) {
        // How many seconds from the starting point to next interrupt.
        ptr = local_ptime + core_num * i;
        node_off = node_osbias[i + local_node_sz];
        tmax = 0;
        for (j = 0; j < core_num; j ++) {
            ptr[j] -= *last_sync_time;
            tmax = tmax < ptr[j] ? ptr[j] : tmax;
        }       
        nnoise = (tmax - node_off) / os_period + 1;
        passed_noise = 0;

        while(nnoise > passed_noise){
            for (k = 0; k < nnoise - passed_noise; k ++) {
                get_urandom(&rand);
                core = rand % core_num;
                ptr[core] += os_detour;
                tmax = tmax < ptr[core]? ptr[core] : tmax;
            }
            passed_noise += k;
            nnoise = (tmax - node_off) / os_period;
        }
        local_final = local_final < tmax ? tmax : local_final;
    }

    MPI_Allreduce(&local_final, &global_final, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    *last_sync_time += global_final;
    for ( i = 0; i < local_sz; i ++){
        local_total_run_time[i] = *last_sync_time;
    }
    if (my_rank == 0){
        free(global_total_run_time);
    }
    free(local_ptime);

    return global_final;
}

void
get_max_time(double *totalrun_time){
    double local_max_time = -1;
    double global_max_time = -1;
    for (int i = 0; i < local_sz; i ++){
        if(totalrun_time[i] > local_max_time)
            local_max_time = totalrun_time[i];
    }
    MPI_Allreduce(&local_max_time, &global_max_time, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    
    for (int i = 0; i < local_sz; i ++){
        totalrun_time[i] = global_max_time;
    }
}

// real time = self run time + childs run time + osnoise
void
__get_real_time(graph_node **ptr, double *local_total_run_time){
    //self run time
    for (int i = 0; i < local_sz; i ++){
        double time = add_noise(ptr[i], i);
        if(ptr[i]->record_flag == 1){
            //start time
            ptr[i]->mrunning_time = local_total_run_time[i];
        }
        local_total_run_time[i] += time;
    }
    graph_node **new_ptr = (graph_node**) malloc(sizeof(graph_node*) * local_sz);

    //childs' run time
    for (int i = 0; i < ptr[0]->child_num; i ++){
        for (int k = 0; k < local_sz; k ++)
            new_ptr[k]=ptr[k]->childs[i];
        for (int j = 0; j < ptr[0]->loop_nums[i]; j ++)
            __get_real_time(new_ptr, local_total_run_time);
    }

    //global sync
    if(ptr[0]->sync == 1){//需要同步
        if(os_switch == 0){//no os noise
            get_max_time(local_total_run_time);
        }else{
            double passed_time = add_os_noise(&last_sync_time, local_total_run_time);
            update_osbias(node_osbias, node_num, os_period, passed_time);
        }
    }

    //record mrunning time
    if(ptr[0]->record_flag == 1){
        for (int i = 0; i < local_sz; i ++){
            //end time
            ptr[i]->mrunning_time = local_total_run_time[i] - ptr[i]->mrunning_time;
            ptr[i]->total_mrunning_time += ptr[i]->mrunning_time;
        }
    }
    free(new_ptr);
    return;
}

void
get_real_time(graph **m_graph,double *local_total_run_time){
    memset(local_total_run_time, 0, sizeof(double) * local_sz);

    graph_node **ptr = (graph_node**) malloc(sizeof(graph_node*) * local_sz);
    for (int i = 0; i < local_sz; i ++){
        ptr[i] = m_graph[i]->root_node;
    }
    
    //get real run time
    __get_real_time(ptr, local_total_run_time);

    //clean
    free(ptr);
}

void
Init_gsl(){
    gsl_rng_env_setup();
    T = gsl_rng_ranlux389;
    r = (gsl_rng**) malloc(sizeof(gsl_rng*) * local_sz);
    seed = (uint64_t*) malloc(sizeof(uint64_t) * local_sz);
    for (int i = 0; i < local_sz; i ++){
        r[i] = gsl_rng_alloc(T);        
        get_urandom(&seed[i]);
        gsl_rng_set(r[i], seed[i]);
    } 
}

void
init_osbias(double **node_osbias, int nnode, float p) {
    if(os_switch == 0)
        return;

    *node_osbias = (double*) malloc(sizeof(double) * nnode);
    if(my_rank == 0){
        uint64_t rand;
        for (int i = 0; i < nnode; i ++) {
            get_urandom(&rand);
            // How many seconds from the starting point to next interrupt.
            (*node_osbias)[i] = p * ((float)rand / (float)UINT64_MAX);
        }
    }
    MPI_Bcast(*node_osbias, nnode, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void
Init_mpi(){
    int err = MPI_Init(NULL, NULL);
    if (err) {
        printf("Failed to initilize MPI environemnt.\n");

        exit(-1);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
}

void
distribute_job(){
    int average = total_core_num / comm_sz; //每个进程平均分配的长度
	int remain = total_core_num % comm_sz;  	
	if(my_rank < remain){
		local_sz = average + 1;
		local_start = (average + 1) * my_rank;
	}
	else{
		local_sz = average;
		local_start = (average + 1) * remain + average * (my_rank - remain);
	}

    //distribute cores
    all_local_sz = (int*) malloc(sizeof(int)*comm_sz);
    all_displs= (int*) malloc(sizeof(int)*comm_sz);
    memset(all_local_sz, 0, comm_sz*sizeof(int));
    memset(all_displs, 0, comm_sz*sizeof(int));

    MPI_Gather(&local_sz, 1, MPI_INT, all_local_sz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(my_rank == 0){
        for ( int j = 0; j < comm_sz ; j ++){	
            if(j > 0){
                all_displs[j] += all_displs[j-1] + all_local_sz[j-1];
            }
        }
    } 

    all_node_sz = (int*) malloc(sizeof(int) * comm_sz);
    all_node_displs = (int*) malloc(sizeof(int) * comm_sz);

    //distribute nodes
    average = node_num / comm_sz; 
	remain = node_num % comm_sz;  	
	if(my_rank < remain){
		local_node_sz = (average + 1);
		local_node_start = (average + 1) * my_rank;
	}
	else{
		local_node_sz = average;
		local_node_start = (average + 1) * remain + average * (my_rank - remain);
	}
    memset(all_node_sz, 0, comm_sz * sizeof(int));
    memset(all_node_displs, 0, comm_sz * sizeof(int));
	for (int i = 0; i < comm_sz; i ++){
		if(i < remain){
			all_node_sz[i] = (average + 1) * core_num;
			all_node_displs[i] = (average + 1) * i * core_num;
		}
		else{
			all_node_sz[i] = average * core_num;
			all_node_displs[i] = (average + 1) * remain * core_num + average * (i - remain) * core_num;
		}
	}
}
void
init_running_time(graph **mgraph){
    for (int i = 0; i < local_sz; i ++){
        for (int j = 0; j < mgraph[i]->node_num; j ++){
            mgraph[i]->node[j]->total_mrunning_time = 0;
            mgraph[i]->node[j]->mrunning_time = 0;
        }
    }
}

void
clean(){
    free(all_local_sz);
    free(all_displs);
    free(all_node_sz);
    free(all_node_displs);
}

int 
main(int argc, char **argv){
    //MPI init
    Init_mpi();
    //parse config file.
    graph **m_graph;
    Init(argc, argv, &m_graph);

    //get theoretical time.
    get_std_time(m_graph[0]);
    for (int i = 0; i < num_run; i ++){
        //init gsl
        Init_gsl();   

        //init running time
        init_running_time(m_graph);
        last_sync_time = 0;
        //add node noise.
        add_node_noise(m_graph);
        //Randomly generate system initial time.
        init_osbias(&node_osbias, node_num, os_period);
        //add kernels noise and os noise and get total run time per core.
        double *local_total_run_time = (double*) malloc(sizeof(double) * local_sz);
        get_real_time(m_graph, local_total_run_time);
        if(my_rank == 0){
            printf("%d, %lf\n",i,local_total_run_time[0]);
        }

        //print running time
        print_running_time(m_graph,i);
        //clean
        free(seed);
        for(int i = 0; i < local_sz; i++){
            gsl_rng_free(r[i]);
        }
        free(node_osbias);
        free(local_total_run_time);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    //clean
    for (int i = 0; i < local_sz; i ++){
       clear_graph(m_graph[i]);
    }
    clean();
    MPI_Finalize();
}