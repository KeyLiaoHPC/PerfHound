#include "frame_init.h"
#include "print_structure.h"

void check(char *name, graph_node *node){
	if(strcmp(name, node->name) == 0){
		printf("%s exist recursive call, please check your config file.\n", name);
		exit(-1);
	}else{
		for (int i = 0; i < node->parent_num; i ++){
			check(name, node->parents[i]);	
		}
	}
}

int add_edge(char *child_name, char *parent_name, int loop_num, graph *m_graph){
	graph_node *child;
	graph_node *parent;
	child = find_node(child_name, m_graph);
	parent = find_node(parent_name, m_graph);
	if(child == NULL){
		printf("could not find %s, please check your config file.\n", child_name);
		exit(-1);
	}
	if(parent == NULL){
		printf("could not find %s, please check your config file.\n", parent_name);
		exit(-1);
	}
	//check(child->name,parent);
	child->parents[child->parent_num] = parent;
	child->parent_num ++;
	parent->childs[parent->child_num] = child;
	parent->loop_nums[parent->child_num] = loop_num;
	parent->child_num ++;
}

int add_node(const char *mnode_name, double mrun_time, noise *m_noise, int sync, int record_flag, graph *m_graph){
	graph_node *new_node = (graph_node*) malloc(sizeof(graph_node));
	char *node_name = (char*) malloc(sizeof(char) * MAX_NAME_SIZE);
	strcpy(node_name, mnode_name);
	new_node->name = node_name;
	new_node->run_time = mrun_time;
	new_node->std_run_time = mrun_time;	
	new_node->m_noise = m_noise;
	new_node->parents = (graph_node**) malloc(sizeof(graph_node*) * MAX_PARENT_NUM);
	new_node->childs = (graph_node**) malloc(sizeof(graph_node*) * MAX_CHILD_NUM);
	new_node->parent_num = 0;
	new_node->child_num = 0;
	new_node->loop_nums = (int*) malloc(sizeof(int) * MAX_CHILD_NUM);
	new_node->sync = sync;
	new_node->record_flag = record_flag;
	new_node->mrunning_time = 0;
	new_node->total_mrunning_time = 0;
	m_graph->node[m_graph->node_num] = new_node;
	if(m_graph->node_num == 0)
		m_graph->root_node = new_node;
	m_graph->node_num ++;
}

void clear_node(graph_node *mNode){
	if(mNode->name){
		free(mNode->name);
	}
	if(mNode->m_noise){
		free(mNode->m_noise->noise_type);
		free(mNode->m_noise->parameters);
	}
	if(mNode->loop_nums){
		free(mNode->loop_nums);
	}
	if(mNode->parents){
		free(mNode->parents);
	}
	if(mNode->childs){
		free(mNode->childs);
	}
}

graph_node* find_node(char *name, graph *m_graph){
	for (int i = 0; i < m_graph->node_num; i ++){
		if(strcmp(m_graph->node[i]->name, name) == 0){
			return m_graph->node[i];
		}
	}
	return NULL;
}

void clear_graph(graph *m_graph){
	for (int i = 0; i < m_graph->node_num; i ++){
		clear_node(m_graph->node[i]);
	}
	if(m_graph->node){
		free(m_graph->node);
	}
}

int read_to_buf(const char *file_name,char *config_buf){
	FILE* fp = fopen(file_name, "r");
	if(!fp){
        printf("Config file %s not found.\n", file_name);
        return 0;
    }
    fread(config_buf, MAX_FILE_SIZE, 1, fp);
    fclose(fp);
	return 1;
}

void parse_noise(cJSON *noise_json, noise **m_noise){
	if(noise_json == NULL){
		if(*m_noise)
			free(*m_noise);
		*m_noise = NULL;
		return;
	}
	cJSON *noise_type;
	cJSON *noise_parameter;
	noise_type = cJSON_GetObjectItem(noise_json, "noiseType");
	(*m_noise)-> noise_type = (char*) malloc(30);
	strcpy((*m_noise)-> noise_type, noise_type->valuestring);
	noise_parameter = cJSON_GetObjectItem(noise_json, "noiseParameters");
	(*m_noise)->parameter_num = cJSON_GetArraySize(noise_parameter);
	(*m_noise)->parameters = (double*)malloc(sizeof(double) * (*m_noise)->parameter_num);
	for (int i = 0;i < (*m_noise)->parameter_num; i ++){
		(*m_noise)->parameters[i] = cJSON_GetArrayItem(noise_parameter, i)->valuedouble;
	}
}

void init_nodes(graph *m_graph,cJSON *config_json){
	//add main
	add_node("main", 0, NULL, 1, 0, m_graph);
	//add functions
	cJSON *functions = cJSON_GetObjectItem(config_json, "functions")->child;
	while(functions != NULL){
		//if need to sync
		int sync = 0;
		//if need to record running time
		int record_flag = 0;
		cJSON *sync_Json = cJSON_GetObjectItem(functions, "sync");
		if(sync_Json != NULL){
			if(strcmp(sync_Json->valuestring, "yes") == 0){
				sync = 1;
			}
		}
		cJSON *recode_Json = cJSON_GetObjectItem(functions, "recordTime");
		if(recode_Json != NULL){
			if(strcmp(recode_Json->valuestring, "yes") == 0){
				record_flag = 1;
			}
		}		
		//The function itself has no running time and noise
		add_node(functions->string, 0, NULL, sync, record_flag, m_graph);
		functions = functions->next;
	}
	//add kernels
	cJSON *kernels = cJSON_GetObjectItem(config_json, "kernels")->child;
	while (kernels != NULL){
		//running time
		double time = 0;
		//if need to record running time
		int record_flag = 0;
		noise *m_noise = (noise*) malloc(sizeof(noise));
		parse_noise(cJSON_GetObjectItem(kernels,"noise"), &m_noise);
		cJSON *run_time_json = cJSON_GetObjectItem(kernels, "runTime");
		if(run_time_json != NULL){
			time = strtod(run_time_json->valuestring, NULL);
		}
	    //record running time
	    cJSON *recode_Json = cJSON_GetObjectItem(kernels, "recordTime");
	    if(recode_Json != NULL){
			if(strcmp(recode_Json->valuestring, "yes") == 0){
				record_flag = 1;
			}
		}
		add_node(kernels->string, time, m_noise, 0, record_flag, m_graph);
		kernels = kernels->next;
	}
}

void add_func_edges(cJSON *functions, graph *m_graph){
	while (functions != NULL){
		//call
		cJSON *call = cJSON_GetObjectItem(functions, "call");
		if(call != NULL){
			int call_num = cJSON_GetArraySize(call);
			char* ptr = functions -> string;
			for (int i = call_num - 1; i >= 0; i --){
				add_edge(cJSON_GetArrayItem(call, i)->valuestring, ptr, 1, m_graph);
				if(cJSON_GetArrayItem(call, i) != NULL){
					ptr = cJSON_GetArrayItem(call, i)->valuestring;
				}
			}
		}
		//loop
		cJSON *loop = cJSON_GetObjectItem(functions, "loop");
		if(loop != NULL){
			call = cJSON_GetObjectItem(loop, "call");
			cJSON *loop_num_json = cJSON_GetObjectItem(loop, "loopNum");
			if(loop_num_json == NULL){
				printf("please set %s's loopNum value.\n", functions->string);
				exit(-1);
			}
			int loop_num = loop_num_json->valueint;
			if(call != NULL){
				int call_num = cJSON_GetArraySize(call);
				char *ptr = functions->string;

				for (int i = call_num - 1; i >= 0; i --){
					//exit(0);
					if(i == call_num - 1){
						add_edge(cJSON_GetArrayItem(call, i)->valuestring, ptr, loop_num, m_graph);
					}
					else{
						add_edge(cJSON_GetArrayItem(call, i)->valuestring, ptr, 1, m_graph);
					}
					if(cJSON_GetArrayItem(call, i) != NULL){
						ptr = cJSON_GetArrayItem(call, i)->valuestring;
					}
				}
			}
		}
		//next function
		functions = functions->next;
	}
}

void init_edges(graph *m_graph,cJSON *config_json){
	//main
	cJSON *program = cJSON_GetObjectItem(config_json, "program");
	if(program != NULL){
		int call_num = cJSON_GetArraySize(program);
		char *ptr = "main";
		for (int i = call_num - 1; i >= 0; i --){
			add_edge(cJSON_GetArrayItem(program, i)->valuestring, ptr, 1, m_graph);
			if(cJSON_GetArrayItem(program, i) != NULL)
			ptr = cJSON_GetArrayItem(program,i)->valuestring;	
		}
	}

	//functions and kernels
	cJSON *functions = cJSON_GetObjectItem(config_json, "functions")->child;
	cJSON *kernels = cJSON_GetObjectItem(config_json, "kernels")->child;
	add_func_edges(functions, m_graph);
	add_func_edges(kernels, m_graph);
}

void init_graph(graph ***m_graph,const char *config_buf){
	cJSON *config_json = cJSON_Parse(config_buf);
	if(config_json == NULL && my_rank == 0){
		printf("Error: parse config file fail. Please check your config file.\n");
		exit(-1);
	}
	*m_graph = (graph**) malloc(sizeof(graph*) * local_sz);

	//init graph
	for (int i = 0; i < local_sz; i ++){
		(*m_graph)[i] = (graph*) malloc(sizeof(graph));
		(*m_graph)[i]->node = (graph_node**) malloc(sizeof(graph_node*) * MAX_NODE_NUM);
		(*m_graph)[i]->node_num = 0;
		(*m_graph)[i]->root_node = NULL;
		//add nodes to graph
		init_nodes((*m_graph)[i], config_json);
		//add edges to graph
		init_edges((*m_graph)[i], config_json);
	}

	//clean
	cJSON_Delete(config_json);
}

void parse_program(graph ***m_graph,const char *file_name){
	char *config_buf = (char*) malloc(MAX_FILE_SIZE);
	if(my_rank == 0)
		if(!read_to_buf(file_name, config_buf))
			return;
	MPI_Bcast(config_buf, MAX_FILE_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

	if (my_rank == 0){
		//print function call structure
    	print_structure(config_buf);
	}
	init_graph(m_graph, config_buf);
	free(config_buf);
	return;
}

void parse_node_config(const char *file_name){
	char *config_buf = (char*) malloc(MAX_FILE_SIZE * sizeof(char));
	if(my_rank == 0){
		read_to_buf(file_name, config_buf);
	}
	MPI_Bcast(config_buf, MAX_FILE_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
	cJSON *config_json = cJSON_Parse(config_buf);
	cJSON *core_json = cJSON_GetObjectItem(config_json, "core");
	cJSON *node_json = cJSON_GetObjectItem(config_json, "node");
	if(core_json == NULL){
		printf("node.json has no core configration.\n");
		exit(-1);
	}else{
		core_num = cJSON_GetObjectItem(core_json, "coreNum")->valueint;
	}
	if(node_json == NULL){
		printf("node.json has no node configration.\n");
		exit(-1);
	}else{
		node_num = cJSON_GetObjectItem(node_json, "nodeNum")->valueint;
		cJSON* noise_json = cJSON_GetObjectItem(node_json, "noise");
		if(noise_json == NULL){
			node_noise = NULL;
		}else{
			node_noise = (noise*) malloc(sizeof(noise));
			parse_noise(noise_json, &node_noise);
		}
	}
	//clean
	cJSON_Delete(config_json);
	free(config_buf);
}

void parse_os_config(const char *file_name){
	char *config_buf = (char*) malloc(MAX_FILE_SIZE * sizeof(char));
	if(my_rank == 0){
		read_to_buf(file_name, config_buf);
	}
	MPI_Bcast(config_buf, MAX_FILE_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
	cJSON *osJson = cJSON_Parse(config_buf);
	cJSON *noise_json = cJSON_GetObjectItem(osJson, "osNoise");
	if(noise_json == NULL){
		os_period = 0;
		os_detour = 0;
		os_switch = 0;
	}else{
		os_switch = 1;
		os_period = strtod(cJSON_GetObjectItem(noise_json, "period")->valuestring, NULL);
		os_detour = strtod(cJSON_GetObjectItem(noise_json, "detour")->valuestring, NULL);		
	}
	//clean
	cJSON_Delete(osJson);
	free(config_buf);
}

