#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <mpi.h>

#define MAX_FILE_SIZE 10000
#define MAX_CHILD_NUM 500
#define MAX_PARENT_NUM 500
#define MAX_NAME_SIZE 200
#define MAX_NODE_NUM 2000

typedef struct noise{
	char* noiseType;
	int parameterNum;
	double* parameters;
}noise;

typedef struct graphNode graphNode;
typedef struct graph graph;

struct graphNode{
	//self information
	char* name;
	double runtime;
	double originRuntime;
	noise* mNoise;
	//parents and childs
	graphNode** parents;
	graphNode** childs;
	int childNum;
	int parentNum;
	//Number of loop per child 
	int* loopNums;
	//sync：1  nonsync: 0 
	int sync;
};

struct graph{
	int nodeNum;
	graphNode** node;
	graphNode* rootNode;//main function
};


//node config
extern int nodeNum;
extern int coreNum;
extern noise* nodeNoise;
extern int totalCoreNum;
//os noise config
extern double osPeriod;
extern double osDetour;
extern int8_t osSwitch; //1 on, 0 down
extern double lastSyncTime;
extern double *node_osbias;
//gsl
extern const gsl_rng_type *T;
extern gsl_rng **r;
uint64_t *seed;
//mpi
extern int my_rank;
extern int commsz;
extern int local_start;
extern int local_sz;
extern int local_node_start;
extern int local_node_sz;
extern int local_node_sz;
extern int* all_local_sz;
extern int* all_displs;
extern int* all_node_sz;
extern int* all_node_displs;


void clearGraph(graph* mGraph);
void clearNode(graphNode* mNode);
void check(char* name, graphNode* node);
int addEdge(char* childName,char* parentName,int loopNum,graph* mGraph);
int addNode(const char* mNodeName,double mRuntime,noise* mNoise,int sync,graph* mgraph);
graphNode* findNode(char* name,graph *mGraph);
void parseProgram(graph*** mGraph,const char* fileName);
void parseNode(const char* fileName);
void parseOs(const char* fileName);
int readToBuf(const char* fileName,char* configBuf);
void initGraph(graph*** mGraph,const char* configBuf);
void initNodes(graph* mGraph,cJSON* configJson);
double GetTheoreticalTime(graph* mGraph);

void distributeJob();