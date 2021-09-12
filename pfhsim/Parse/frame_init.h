#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#define MAX_FILE_SIZE 10000

typedef struct FrameJson{
    cJSON* configJson;
    cJSON* cluster;
    cJSON* node;
    cJSON* core;
    cJSON* function1;
    cJSON* function2;
    cJSON* function3;
    cJSON* kernel;
    cJSON* k1;
    cJSON* k2;
    cJSON* k3;
	cJSON* instruction;
}FrameJson;
typedef struct noise{
	char* noiseType;
	int parameterNum;
	double* parameters;
}noise;
typedef struct parameters{
	//node parameters
	int numRun;
	int nodeNum;
	noise nodeNoise;
	//core parameters
	int coreNum;
	noise coreNoise;
	//function parameters
	int loopNum;
	double Tf1;
	noise F1noise;
	double Tf3;
	//F2 Kernel parameters
	int F2kernelLoopNum;
	double Tk1;
	noise k1noise;
	double Tk3;
	int K2InstructionNum;
	double Ti;
	noise Insnoise;
}parameters;

int readToBuf(const char* fileName,char* configBuf);
int initFrame(FrameJson* Frame,char* configBuf);
void parse_noise(cJSON* noiseJson, noise* mNoise );
int init_par(parameters* mparameters,FrameJson Frame);
int Parse(parameters* mparameters,const char* fileName);