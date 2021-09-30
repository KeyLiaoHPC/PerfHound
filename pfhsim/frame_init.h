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
	char *noise_type;
	int parameter_num;
	double *parameters;
}noise;

typedef struct graph_node graph_node;
typedef struct graph graph;

struct graph_node{
	//self information
	char *name;
	double run_time;
	double std_run_time;
	noise *m_noise;
	//parents and childs
	graph_node **parents;
	graph_node **childs;
	int child_num;
	int parent_num;
	//Number of loop per child 
	int *loop_nums;
	//sync：1  nonsync: 0 
	int sync;
};

struct graph{
	int node_num;
	graph_node **node;
	graph_node *root_node;//main function
};


//node config
extern int node_num;
extern int core_num;
extern noise *node_noise;
extern int total_core_num;
//os noise config
extern double os_period;
extern double os_detour;
extern int8_t os_switch; //1 on, 0 down
extern double last_sync_time;
extern double *node_osbias;
//gsl
extern const gsl_rng_type *T;
extern gsl_rng **r;
uint64_t *seed;
//mpi
extern int my_rank;
extern int comm_sz;
extern int local_start;
extern int local_sz;
extern int local_node_start;
extern int local_node_sz;
extern int local_node_sz;
extern int *all_local_sz;
extern int *all_displs;
extern int *all_node_sz;
extern int *all_node_displs;


void clear_graph(graph *m_graph);
void clear_node(graph_node *mnode);
void check(char *name, graph_node *node);
int add_edge(char *child_name, char *parent_name, int loop_num, graph *m_graph);
int add_node(const char *mnode_name, double mrun_time, noise *m_noise, int sync, graph *m_graph);
graph_node* find_node(char *name, graph *m_graph);
void parse_program(graph ***m_graph, const char *file_name);
void parse_node_config(const char *file_name);
void parse_os_config(const char *file_name);
int read_to_buf(const char *file_name, char *config_buf);
void init_graph(graph ***m_graph, const char *config_buf);
void init_nodes(graph *m_graph, cJSON *config_json);
double get_std_time(graph *m_graph);
void distribute_job();