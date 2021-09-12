#include "frame_init.h"

int readToBuf(const char* fileName,char* configBuf){
	FILE* fp = fopen(fileName,"r");
	if(!fp){
        printf("Config file %s not found.\n",fileName);
        return 0;
    }
    fread(configBuf,MAX_FILE_SIZE,1,fp);
    fclose(fp);
	return 1;
}

int initFrame(FrameJson* Frame,char* configBuf){
	Frame->configJson = NULL;
	Frame->cluster = NULL;
	Frame->node = NULL;
	Frame->core = NULL;
	Frame->function1 = NULL;
	Frame->function2 = NULL;
	Frame->function3 = NULL;
	Frame->kernel = NULL;
	Frame->k1 = NULL;
	Frame->k2 = NULL;
	Frame->k3 = NULL;
	Frame->instruction = NULL;
	Frame->configJson = cJSON_Parse(configBuf);
	if(Frame->configJson == NULL){
		printf("parse config.json fail. Please check your config file.\n");
		return -1;
	}
	Frame->cluster = cJSON_GetObjectItem(Frame->configJson,"cluster");
	if(Frame->cluster == NULL){
		printf("fail to find item: cluster.\n");
		return -1;
	}
	Frame->node = cJSON_GetObjectItem(Frame->cluster,"node");
	if(Frame->node == NULL){
		printf("fail to find item: node.\n");
		return -1;
	}
	Frame->core = cJSON_GetObjectItem(Frame->node,"core");
	if(Frame->core == NULL){
		printf("fail to find item: core.\n");
		return -1;
	}
	Frame->function1 = cJSON_GetObjectItem(Frame->core,"function1");
	if(Frame->function1 == NULL){
		printf("fail to find item: function1.\n");
		return -1;
	}
	Frame->function2 = cJSON_GetObjectItem(Frame->core,"function2");
	if(Frame->function2 == NULL){
		printf("fail to find item: function2.\n");
		return -1;
	}
	Frame->function3 = cJSON_GetObjectItem(Frame->core,"function3");
	if(Frame->function3 == NULL){
		printf("fail to find item: function3.\n");
		return -1;
	}
	Frame->kernel = cJSON_GetObjectItem(Frame->function2,"kernel");
	if(Frame->kernel == NULL){
		printf("fail to find item: kernel.\n");
		return -1;
	}
	Frame->k1 = cJSON_GetObjectItem(Frame->kernel,"k1");
	if(Frame->k1 == NULL){
		printf("fail to find item: k1.\n");
		return -1;
	}
	Frame->k2 = cJSON_GetObjectItem(Frame->kernel,"k2");
	if(Frame->k2 == NULL){
		printf("fail to find item: k2.\n");
		return -1;
	}
	Frame->k3 = cJSON_GetObjectItem(Frame->kernel,"k3");
	if(Frame->k3 == NULL){
		printf("fail to find item: k3.\n");
		return -1;
	}
	Frame->instruction = cJSON_GetObjectItem(Frame->k2,"instruction");
	if(Frame->instruction == NULL){
		printf("fail to find item: instruction.\n");
		return -1;
	}
	return 0;
}
void parse_noise(cJSON* noiseJson, noise* mNoise ){
	cJSON* noiseType;
	cJSON* noiseParameter;
	noiseType = cJSON_GetObjectItem(noiseJson, "noiseType");
	mNoise-> noiseType = (char*) malloc(20);
	strcpy(mNoise-> noiseType,noiseType->valuestring);
	noiseParameter = cJSON_GetObjectItem(noiseJson, "noiseParameter");
	mNoise->parameterNum = cJSON_GetArraySize(noiseParameter);
	mNoise->parameters = (double*)malloc(sizeof(double)*mNoise->parameterNum);
	cJSON* pari = noiseParameter->child;
	for(int i = 0;i < mNoise->parameterNum; i++){
		mNoise->parameters[i] = strtod(pari->valuestring,NULL);
		pari = pari->next;
	}
}

int init_par(parameters* mparameters,FrameJson Frame){
	mparameters->numRun = atoi(cJSON_GetObjectItem(Frame.configJson,"numRun")->valuestring);
	mparameters->nodeNum = atoi(cJSON_GetObjectItem(Frame.cluster,"nodeNum")->valuestring);
	parse_noise(cJSON_GetObjectItem(Frame.cluster,"nodeNoise"),&mparameters->nodeNoise );
	mparameters->coreNum = atoi(cJSON_GetObjectItem(Frame.node,"coreNum")->valuestring);
	parse_noise(cJSON_GetObjectItem(Frame.node,"osNoise"),&mparameters->coreNoise);
	mparameters->loopNum = atoi(cJSON_GetObjectItem(Frame.core,"loopNum")->valuestring);
	mparameters->Tf1 = strtod(cJSON_GetObjectItem(Frame.function1,"runTime")->valuestring,NULL);
	parse_noise(cJSON_GetObjectItem(Frame.function1,"noise"),&mparameters->F1noise);
	mparameters->Tf3 = strtod(cJSON_GetObjectItem(Frame.function3,"runTime")->valuestring,NULL);
	mparameters->F2kernelLoopNum = atoi(cJSON_GetObjectItem(Frame.function2,"kernelLoopNum")->valuestring);
	mparameters->Tk1 = strtod(cJSON_GetObjectItem(Frame.k1,"runTime")->valuestring,NULL);
	parse_noise(cJSON_GetObjectItem(Frame.k1,"noise"),&mparameters->k1noise);
	mparameters->Tk3 = strtod(cJSON_GetObjectItem(Frame.k3,"runTime")->valuestring,NULL);
	mparameters->K2InstructionNum = atoi(cJSON_GetObjectItem(Frame.k2,"instructionNum")->valuestring);
	mparameters->Ti = strtod(cJSON_GetObjectItem(Frame.instruction,"runTime")->valuestring,NULL);
	parse_noise(cJSON_GetObjectItem(Frame.instruction,"noise"),&mparameters->Insnoise);
}

int Parse(parameters* mparameters,const char* fileName){
	FrameJson Frame;
	char* configBuf = (char*)malloc(MAX_FILE_SIZE);
	if(!readToBuf(fileName,configBuf))
		return -1;
	if(initFrame(&Frame,configBuf) == -1)
		return -1;
	init_par(mparameters,Frame);
	cJSON_Delete(Frame.configJson);
	return 1;
}

/*int main(){
	parameters mparameters;
	Parse(&mparameters,"../config.json");
}*/
