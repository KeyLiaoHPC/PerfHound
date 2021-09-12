#include <mpi.h>
#include "../Parse/frame_init.h"

double Tf1;
double Tf3;
double Tk1; 
double Tk3;
double Ti;
int Lf;
int Lk;
int Li;
int NRUN;
int NP_NODE;               // # of cores per node.
int NNODE;                 // # of nodes.
double NORM_SIGMA;
double NORM_CUT;
int PARETO_K;
double SYSVAR_P;              // Invoking period (sec) of os noise.
double SYSVAR_T;         // Time(sec) consumed by the os noise.
int nproc, myrank;

void init_varilabes(parameters mparameters);