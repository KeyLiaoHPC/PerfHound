#include "frame_init.h"

void check(char* name, graphNode* node){
	if(strcmp(name,node->name) == 0){
		printf("%s exist recursive call, please check your config file.\n",name);
		exit(-1);
	}else{
		for (int i = 0; i < node->parentNum; i++){
			check(name,node->parents[i]);	
		}
	}
}

int addEdge(char* childName,char* parentName,int loopNum,graph* mGraph){
	graphNode* child;
	graphNode* parent;
	child = findNode(childName,mGraph);
	parent = findNode(parentName,mGraph);
	if(child == NULL){
		printf("could not find %s, please check your config file.\n",childName);
		exit(-1);
	}
	if(parent == NULL){
		printf("could not find %s, please check your config file.\n",parentName);
		exit(-1);
	}
	//check(child->name,parent);
	child->parents[child->parentNum] = parent;
	child->parentNum ++;
	parent->childs[parent->childNum] = child;
	parent->loopNums[parent->childNum] = loopNum;
	parent->childNum ++;
}

int addNode(const char* mNodeName,double mRuntime,noise* mNoise,int sync, graph* mgraph){
	graphNode* newNode = (graphNode*)malloc(sizeof(graphNode));
	char* nodeName = (char*) malloc(sizeof(char)*MAX_NAME_SIZE);
	strcpy(nodeName,mNodeName);
	newNode->name = nodeName;
	newNode->runtime = mRuntime;
	newNode->originRuntime = mRuntime;	
	newNode->mNoise = mNoise;
	newNode->parents = (graphNode**) malloc(sizeof(graphNode*)*MAX_PARENT_NUM);
	newNode->childs = (graphNode**) malloc(sizeof(graphNode*)*MAX_CHILD_NUM);
	newNode->parentNum = 0;
	newNode->childNum = 0;
	newNode->loopNums = (int*) malloc(sizeof(int)*MAX_CHILD_NUM);
	newNode->sync = sync;
	mgraph->node[mgraph->nodeNum] = newNode;
	if(mgraph->nodeNum == 0)
		mgraph->rootNode = newNode;
	mgraph->nodeNum ++;
}

void clearNode(graphNode* mNode){
	if(mNode->name)
		free(mNode->name);
	if(mNode->mNoise){
		free(mNode->mNoise->noiseType);
		free(mNode->mNoise->parameters);
	}
	if(mNode->loopNums)
		free(mNode->loopNums);
	if(mNode->parents)
		free(mNode->parents);
	if(mNode->childs)
		free(mNode->childs);
}

graphNode* findNode(char* name,graph *mGraph){
	for(int i = 0; i < mGraph->nodeNum; i++){
		if(strcmp(mGraph->node[i]->name,name) == 0)
			return mGraph->node[i];
	}
	return NULL;
}

void clearGraph(graph* mGraph){
	for (int i = 0; i < mGraph->nodeNum; i++)
		clearNode(mGraph->node[i]);
	if(mGraph->node)
		free(mGraph->node);
}

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

void parse_noise(cJSON* noiseJson, noise** mNoise){
	if(noiseJson == NULL){
		if(*mNoise)
			free(*mNoise);
		*mNoise = NULL;
		return;
	}
	cJSON* noiseType;
	cJSON* noiseParameter;
	noiseType = cJSON_GetObjectItem(noiseJson, "noiseType");
	(*mNoise)-> noiseType = (char*) malloc(30);
	strcpy((*mNoise)-> noiseType,noiseType->valuestring);
	noiseParameter = cJSON_GetObjectItem(noiseJson, "noiseParameters");
	(*mNoise)->parameterNum = cJSON_GetArraySize(noiseParameter);
	(*mNoise)->parameters = (double*)malloc(sizeof(double)*(*mNoise)->parameterNum);
	for(int i = 0;i < (*mNoise)->parameterNum; i++){
		(*mNoise)->parameters[i] = cJSON_GetArrayItem(noiseParameter,i)->valuedouble;
	}
}

void initNodes(graph* mGraph,cJSON* configJson){
	//add main
	addNode("main",0,NULL,1,mGraph);
	//add functions
	cJSON* functions = cJSON_GetObjectItem(configJson,"functions")->child;
	while(functions != NULL){
		//sync
		int sync = 0;
		cJSON* syncJson = cJSON_GetObjectItem(functions,"sync");
		if(syncJson != NULL)
			if(strcmp(syncJson->valuestring,"yes") == 0)
				sync = 1;
		//The function itself has no running time and noise
		addNode(functions->string,0,NULL,sync,mGraph);
		functions = functions->next;
	}
	//add kernels
	cJSON* kernels = cJSON_GetObjectItem(configJson,"kernels")->child;
	while (kernels != NULL){
		noise* mNoise = (noise*)malloc(sizeof(noise));
		parse_noise(cJSON_GetObjectItem(kernels,"noise"),&mNoise);
		cJSON* runTimeJson = cJSON_GetObjectItem(kernels,"runTime");
		double time = 0;
		if(runTimeJson != NULL)
			time = strtod(runTimeJson->valuestring,NULL);
		addNode(kernels->string,time,mNoise,0,mGraph);
		kernels = kernels->next;
	}
}

void addFuncEdges(cJSON* functions,graph* mGraph){
	while (functions != NULL){
		//noloop
		cJSON* call = cJSON_GetObjectItem(functions,"call");
		if(call != NULL){
			int callNum = cJSON_GetArraySize(call);
			char* ptr = functions -> string;
			for(int i = callNum - 1; i >= 0; i--){
				addEdge(cJSON_GetArrayItem(call,i)->valuestring,ptr,1,mGraph);
				//printf("add : %s -> %s\n",cJSON_GetArrayItem(call,i)->valuestring,ptr);
				if(cJSON_GetArrayItem(call,i) != NULL)
					ptr = cJSON_GetArrayItem(call,i)->valuestring;
			}
		}
		//loop
		cJSON* loop = cJSON_GetObjectItem(functions,"loop");
		if(loop != NULL){
			call = cJSON_GetObjectItem(loop,"call");
			cJSON* loopNumJson = cJSON_GetObjectItem(loop,"loopNum");
			if(loopNumJson == NULL){
				printf("please set %s's loopNum value.\n",functions->string);
				exit(-1);
			}
			int loopNum = loopNumJson->valueint;
			if(call != NULL){
				int callNum = cJSON_GetArraySize(call);
				char* ptr = functions -> string;

				for(int i = callNum - 1; i >= 0; i--){
					//exit(0);
					if(i == callNum -1)
						addEdge(cJSON_GetArrayItem(call,i)->valuestring,ptr,loopNum,mGraph);
					else
						addEdge(cJSON_GetArrayItem(call,i)->valuestring,ptr,1,mGraph);
					if(cJSON_GetArrayItem(call,i) != NULL)
						ptr = cJSON_GetArrayItem(call,i)->valuestring;
				}
			}
		}
		//next func
		functions = functions->next;
	}
}

void initEdges(graph* mGraph,cJSON* configJson){
	//main
	cJSON* program = cJSON_GetObjectItem(configJson,"program");
	if(program != NULL){
		int callNum = cJSON_GetArraySize(program);
		char *ptr = "main";
		for(int i = callNum - 1; i >= 0; i--){
			addEdge(cJSON_GetArrayItem(program,i)->valuestring,ptr,1,mGraph);
			if(cJSON_GetArrayItem(program,i) != NULL)
			ptr = cJSON_GetArrayItem(program,i)->valuestring;	
		}
	}
	//functions and kernels
	cJSON* functions = cJSON_GetObjectItem(configJson,"functions")->child;
	cJSON* kernels = cJSON_GetObjectItem(configJson,"kernels")->child;
	addFuncEdges(functions,mGraph);
	addFuncEdges(kernels,mGraph);
}

void initGraph(graph*** mGraph,const char* configBuf){
	cJSON* configJson = cJSON_Parse(configBuf);
	if(configJson == NULL && my_rank == 0){
		printf("Error: parse config file fail. Please check your config file.\n");
		exit(-1);
	}

	*mGraph = (graph**) malloc(sizeof(graph*) * local_sz);
	//init graph
	for (int i = 0; i < local_sz; i++){
		(*mGraph)[i] = (graph*) malloc(sizeof(graph));
		(*mGraph)[i]->node = (graphNode**) malloc(sizeof(graphNode*)*MAX_NODE_NUM);
		(*mGraph)[i]->nodeNum = 0;
		(*mGraph)[i]->rootNode = NULL;

		//add nodes to graph
		initNodes((*mGraph)[i],configJson);
		//add edges to graph
		initEdges((*mGraph)[i],configJson);
	}


	//clean
	cJSON_Delete(configJson);
}

void parseProgram(graph*** mGraph,const char* fileName){
	char* configBuf = (char*)malloc(MAX_FILE_SIZE);
	if(my_rank == 0)
		if(!readToBuf(fileName,configBuf))
			return;
	MPI_Bcast(configBuf, MAX_FILE_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

	initGraph(mGraph,configBuf);
	free(configBuf);
	return;
}

void parseNode(const char* fileName){
	char* configBuf = (char*) malloc(MAX_FILE_SIZE*sizeof(char));
	if(my_rank == 0)
		readToBuf(fileName,configBuf);
	MPI_Bcast(configBuf, MAX_FILE_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
	cJSON* configJson = cJSON_Parse(configBuf);
	
	cJSON* coreJson = cJSON_GetObjectItem(configJson,"core");
	cJSON* nodeJson = cJSON_GetObjectItem(configJson,"node");
	if(coreJson == NULL){
		printf("node.json has no core configration.\n");
		exit(-1);
	}else{
		coreNum = cJSON_GetObjectItem(coreJson,"coreNum")->valueint;
	}
	if(nodeJson == NULL){
		printf("node.json has no node configration.\n");
		exit(-1);
	}else{
		nodeNum = cJSON_GetObjectItem(nodeJson,"nodeNum")->valueint;
		cJSON* noiseJson = cJSON_GetObjectItem(nodeJson,"noise");
		if(noiseJson == NULL)
			nodeNoise = NULL;
		else{
			nodeNoise = (noise*) malloc(sizeof(noise));
			parse_noise(noiseJson,&nodeNoise);
		}
	}
	//clean
	cJSON_Delete(configJson);
	free(configBuf);
}

void parseOs(const char* fileName){
	char* configBuf = (char*) malloc(MAX_FILE_SIZE*sizeof(char));
	if(my_rank == 0)
		readToBuf(fileName,configBuf);
	MPI_Bcast(configBuf, MAX_FILE_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

	cJSON* osJson = cJSON_Parse(configBuf);
	cJSON* noiseJson = cJSON_GetObjectItem(osJson,"osNoise");
	if(noiseJson == NULL){
		osPeriod = 0;
		osDetour = 0;
		osSwitch = 0;
	}else{
		osSwitch = 1;
		osPeriod = strtod(cJSON_GetObjectItem(noiseJson,"period")->valuestring,NULL);
		osDetour = strtod(cJSON_GetObjectItem(noiseJson,"detour")->valuestring,NULL);		
	}
	//clean
	cJSON_Delete(osJson);
	free(configBuf);
}

/*int main(){
	graph mGraph;
	Parse(&mGraph,"./config.json");
	int theoreticalTime = GetTheoreticalTime(&mGraph);

}*/
