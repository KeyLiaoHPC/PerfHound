#include "frame_init.h"


double 
get_theoretical_time(graphNode* node){
	double time = 0;
	time += node->runtime;
	for (int i = 0; i < node->childNum; i++){
		time += get_theoretical_time(node->childs[i]) * node->loopNums[i];
	}
	return time;
}

double 
GetTheoreticalTime(graph* mGraph){
    if(my_rank == 0){
        graphNode* main = mGraph->rootNode;
        double time = 0;
        time = get_theoretical_time(main);
        printf("Theoretical Time: %lf seconds\n",time);
    }
}

void 
Init(int argc,char** argv,graph*** mGraph){
    parseNode("./config/node.json");
    parseOs("./config/os.json");   
    
    //distribute sim core
    totalCoreNum = coreNum * nodeNum;
    distributeJob();

    //parse Program
    parseProgram(mGraph,"./config/program.json"); 
    if(my_rank == 0)
        printf("Nodes: %d, Cores: %d\n",nodeNum,coreNum);
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


    sigma = 3 * nodeNoise->parameters[0]; // 3sigma for +-5% of 99.9% ndoes.
    ucut = 0 + nodeNoise->parameters[1];  // Cutoff at +NORM_CUT
    dcut = 0 - nodeNoise->parameters[1];  // Cutoff at -NORM_CUT

    for (i = 0; i < n; i ++) {
        pnode[i] = gsl_ran_gaussian(mr, sigma);
        pnode[i] = pnode[i] > ucut? ucut: pnode[i];
        pnode[i] = pnode[i] < dcut? dcut: pnode[i];
        pnode[i] += 1;
    } 
    gsl_rng_free(mr);

    return 0;
}

void
nodeNoiseDisturb(graph** mGraph){
    if(nodeNoise == NULL) //no node noise
        return;

    double* node_perf = (double*) malloc(sizeof(double) * nodeNum);
    if(my_rank == 0)
        get_nodevar(node_perf, nodeNum);
    MPI_Bcast(node_perf, nodeNum, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for(int i = 0; i < local_sz; i++){
        int simnode = (i + local_start) / coreNum;
        for (int j = 0; j < mGraph[i]->nodeNum ; j++){
            mGraph[i]->node[j]->runtime = mGraph[i]->node[j]->originRuntime / node_perf[simnode];  
        }
    }
    /*if (my_rank == 0){
        printf("Performance factor for node 0-%d: ",nodeNum-1); 
        for (int j = 0; j < nodeNum ; j++)
            printf("%lf, ",node_perf[j]-1);
        printf("\n");   
    }*/
    free(node_perf);
}

double
selfNoiseDisturb(graphNode* node,int i){
    //gsl_rng_set(r[i], seed[i]);
    if(node->mNoise == NULL || node->runtime == 0) //no noise
        return node->runtime;

    if(strcmp(node->mNoise->noiseType,"pareto") == 0){//pareto distribution
        double s = gsl_ran_pareto(r[0], node->mNoise->parameters[0], 1);
        s = s > 2? 2: s;
        return(node->runtime * s);
    }
    return node->runtime; 
}

void
update_osbias(double *node_osbias, int nnode, int p, double passed_time) {
    int passed;
    for (int i = 0; i < nnode; i ++) {
        passed = (node_osbias[i] + passed_time) / p; 
        node_osbias[i] = node_osbias[i] + passed_time - passed * p;
    }
}

void
addOsNoise(double* lastSyncTime, double* localTotalRunTime,double* passedTime){
    uint64_t rand;
    double node_off,tmax,localFinal = 0,globalFinal = 0;
    int nnoise,passed_noise,core,i,j,k;
    double* ptr;

    //gather local run time
    double* globalTotalRunTime;
    if (my_rank == 0){
        globalTotalRunTime = (double*) malloc(sizeof(double) * coreNum * nodeNum);
    }
    MPI_Gatherv(localTotalRunTime, local_sz, MPI_DOUBLE, globalTotalRunTime, all_local_sz, all_displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //distribute job
    double* localPTime = (double*) malloc(sizeof(double) * local_node_sz * coreNum);
    MPI_Scatterv(globalTotalRunTime,all_node_sz,all_node_displs,MPI_DOUBLE,localPTime,local_node_sz * coreNum,MPI_DOUBLE,0,MPI_COMM_WORLD);

    //osInitialTime,osPeriod,osDetour,lastSyncTime, now time
    for (i = 0; i < local_node_sz; i ++) {
        // hypothesis: each node start at a random time in a period.
        //get_urandom(&rand);
        // How many seconds from the starting point to next interrupt.
        ptr = localPTime + coreNum * i;
        node_off = node_osbias[i + local_node_sz];
        tmax = 0;
        for (j = 0; j < coreNum; j ++) {
            ptr[j] -= *lastSyncTime;
            tmax = tmax < ptr[j] ? ptr[j] : tmax;
        }       
        nnoise = (tmax - node_off) / osPeriod + 1;
        passed_noise = 0;

        while(nnoise > passed_noise){
            for (k = 0; k < nnoise - passed_noise; k ++) {
                get_urandom(&rand);
                core = rand % coreNum;
                ptr[core] += osDetour;
                tmax = tmax < ptr[core]? ptr[core] : tmax;
            }
            passed_noise += k;
            nnoise = (tmax - node_off) / osPeriod;
        }
        localFinal = localFinal < tmax ? tmax : localFinal;
    }

    MPI_Allreduce(&localFinal,&globalFinal,1,MPI_DOUBLE,MPI_MAX,MPI_COMM_WORLD);

    *passedTime = globalFinal;
    *lastSyncTime += globalFinal;
    for ( i = 0; i < local_sz; i++){
        localTotalRunTime[i] = *lastSyncTime;
    }
    if (my_rank == 0){
        free(globalTotalRunTime);
    }
    free(localPTime);
}

void
setMaxTime(double* totalRunTime){
    double localMaxTime = -1;
    double globalMaxTime = -1;
    for(int i = 0;i < local_sz; i++){
        if(totalRunTime[i] > localMaxTime)
            localMaxTime = totalRunTime[i];
    }
    MPI_Allreduce(&localMaxTime,&globalMaxTime,1,MPI_DOUBLE,MPI_MAX,MPI_COMM_WORLD);
    
    for(int i = 0;i < local_sz; i++){
        totalRunTime[i] = globalMaxTime;
    }
}

// real time = self run time + childs run time + osnoise
void
get_real_time(graphNode** ptr, double* localTotalRunTime){
    //self run time
    for(int i = 0; i < local_sz; i++){
        localTotalRunTime[i] += selfNoiseDisturb(ptr[i],i);
    }
    graphNode** new_ptr = (graphNode**)malloc(sizeof(graphNode*) * local_sz);

    //childs' run time
    for(int i = 0; i < ptr[0]->childNum; i++){
        for(int k = 0; k < local_sz; k++)
            new_ptr[k]=ptr[k]->childs[i];
        for (int j = 0; j < ptr[0]->loopNums[i]; j++)
            get_real_time(new_ptr,localTotalRunTime);
    }

    // sync
    if(ptr[0]->sync == 1){//需要同步
        if(osSwitch == 0){//no os noise
            setMaxTime(localTotalRunTime);
        }else{
            double passedTime;
            addOsNoise(&lastSyncTime,localTotalRunTime,&passedTime);
            update_osbias(node_osbias, nodeNum, osPeriod, passedTime);
        }
    }
    free(new_ptr);
    return;
}

void
getRealTime(graph** mGraph,double* localTotalRunTime){
    memset(localTotalRunTime,0,sizeof(double) * local_sz);

    graphNode **ptr = (graphNode**) malloc(sizeof(graphNode*) * local_sz);
    for (int i = 0; i < local_sz; i++){
        ptr[i] = mGraph[i] ->rootNode;
    }
    
    //get real run time
    get_real_time(ptr,localTotalRunTime);

    //clean
    free(ptr);
}

void
Init_gsl(){
    gsl_rng_env_setup();
    T = gsl_rng_ranlux389;
    r = (gsl_rng**) malloc(sizeof(gsl_rng*) * local_sz);
    seed = (uint64_t*) malloc(sizeof(uint64_t) * local_sz);
    for (int i = 0; i < local_sz; i++){
        r[i] = gsl_rng_alloc(T);        
        get_urandom(&seed[i]);
        gsl_rng_set(r[i], seed[i]);
    } 
}

void
init_osbias(double **node_osbias, int nnode, float p) {
    if(osSwitch == 0)
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
    MPI_Comm_size(MPI_COMM_WORLD, &commsz);
}

void
distributeJob(){
    int average = totalCoreNum / commsz; //每个进程平均分配的长度
	int remain = totalCoreNum % commsz;  	
	if(my_rank < remain){
		local_sz = average + 1;
		local_start = (average + 1) * my_rank;
	}
	else{
		local_sz = average;
		local_start = (average + 1) * remain + average * (my_rank - remain);
	}

    //distribute cores
    all_local_sz = (int*) malloc(sizeof(int)*commsz);
    all_displs= (int*) malloc(sizeof(int)*commsz);
    memset(all_local_sz,0,commsz*sizeof(int));
    memset(all_displs,0,commsz*sizeof(int));

    MPI_Gather(&local_sz,1,MPI_INT,all_local_sz,1,MPI_INT,0,MPI_COMM_WORLD);
    if(my_rank == 0){
        for(int j = 0; j < commsz ; j++){	
            if(j > 0){
                all_displs[j] += all_displs[j-1] + all_local_sz[j-1];
            }
        }
    } 

    all_node_sz = (int*) malloc(sizeof(int)*commsz);
    all_node_displs = (int*) malloc(sizeof(int)*commsz);

    //distribute nodes
    average = nodeNum / commsz; 
	remain = nodeNum % commsz;  	
	if(my_rank < remain){
		local_node_sz = (average + 1);
		local_node_start = (average + 1) * my_rank;
	}
	else{
		local_node_sz = average;
		local_node_start = (average + 1) * remain + average * (my_rank - remain);
	}
    memset(all_node_sz,0,commsz*sizeof(int));
    memset(all_node_displs,0,commsz*sizeof(int));
	for(int i = 0; i < commsz; i++){
		if(i < remain){
			all_node_sz[i] = (average + 1) * coreNum;
			all_node_displs[i] = (average + 1) * i * coreNum;
		}
		else{
			all_node_sz[i] = average * coreNum;
			all_node_displs[i] = (average + 1) * remain * coreNum + average * (i - remain) * coreNum;
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

//node configration
int nodeNum;
int coreNum;
noise* nodeNoise;
int totalCoreNum;
//os noise configration
int8_t osSwitch;
double osPeriod;
double osDetour;
double lastSyncTime = 0;
double *node_osbias;
//gsl 
const gsl_rng_type *T;
gsl_rng **r;
uint64_t *seed;
//mpi
int my_rank;
int commsz;
int local_start;
int local_sz;
int local_node_start;
int local_node_sz;
int* all_local_sz;
int* all_displs;
int* all_node_sz;
int* all_node_displs;

int 
main(int argc, char **argv){
    //MPI init
    Init_mpi();

    //parse config file.
    graph** mGraph;
    Init(argc,argv,&mGraph);

    //get theoretical time.
    GetTheoreticalTime(mGraph[0]);

    int NUMRUN = 200;
    for (int i = 0; i < NUMRUN; i++){

        //add node noise.
        nodeNoiseDisturb(mGraph);

        //init gsl
        Init_gsl();   

        lastSyncTime = 0;

        //Randomly generate system initial time.
        init_osbias(&node_osbias, nodeNum, osPeriod);
        
        //add kernels noise and os noise and get total run time per core.
        double* localTotalRunTime = (double*) malloc(sizeof(double) * local_sz);
        getRealTime(mGraph,localTotalRunTime);

        if(my_rank == 0)
            printf("%d, %lf\n",i,localTotalRunTime[0]);

        //clean
        free(seed);
        for(int i = 0; i < local_sz; i++)
            gsl_rng_free(r[i]);
        free(node_osbias);
        free(localTotalRunTime);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    //clean
    for (int i = 0; i < local_sz; i++){
       clearGraph(mGraph[i]);
    }
    clean();

    MPI_Finalize();
}