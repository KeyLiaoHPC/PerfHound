#include "print_structure.h"

void 
add_structure_edges(cJSON *functions, graph *mGraph){
	while (functions != NULL){
		//noloop
		cJSON *call = cJSON_GetObjectItem(functions, "call");
		if(call != NULL){
			int call_num = cJSON_GetArraySize(call);
			for (int i = 0; i < call_num; i++){
				char *child_name = cJSON_GetArrayItem(call, i)->valuestring;
				//printf("add : %s -> %s\n", child_name, functions->string);
				add_edge(child_name, functions->string, 1, mGraph);
			}
		}
		//loop
		cJSON* loop = cJSON_GetObjectItem(functions, "loop");
		if(loop != NULL){
			call = cJSON_GetObjectItem(loop, "call");
			cJSON *loopnum_json = cJSON_GetObjectItem(loop, "loopNum");
			if(loopnum_json == NULL){
				printf("please set %s's loopNum value.\n", functions->string);
				exit(-1);
			}
			int loopnum = loopnum_json->valueint;
			if(call != NULL){
				int call_num = cJSON_GetArraySize(call);
				for (int i = 0; i < call_num; i++){
					char* child_name = cJSON_GetArrayItem(call, i)->valuestring;
					//printf("add : %s -> %s\n", child_name, functions->string);
					add_edge(child_name, functions->string, loopnum, mGraph);
				}
			}
		}
		//next func
		functions = functions->next;
	}
}

void
init_structure_edges(graph *structure_graph, cJSON *config_json){
	//main
	cJSON *program = cJSON_GetObjectItem(config_json, "program");
	if(program != NULL){
		int call_num = cJSON_GetArraySize(program);
		for (int i = 0; i < call_num; i ++){
			char *childName = cJSON_GetArrayItem(program, i)->valuestring;
			add_edge(childName, "main", 1, structure_graph);
		}
	}
	//functions and kernels
	cJSON* functions = cJSON_GetObjectItem(config_json, "functions")->child;
	cJSON* kernels = cJSON_GetObjectItem(config_json, "kernels")->child;
	add_structure_edges(functions, structure_graph);
	add_structure_edges(kernels, structure_graph);    
}


/*
* Generate function call graph
*/
void
generate_call_structure(graph *structure_graph, cJSON *config_json){
    //init structure_graph
    structure_graph->node = (graph_node**) malloc(sizeof(graph_node*) * MAX_NODE_NUM); 
    structure_graph->node_num = 0;
    structure_graph->root_node = NULL;
    //add nodes
    init_nodes(structure_graph, config_json);
    //add edges
    init_structure_edges(structure_graph, config_json);   
}

void
__parse_call_structure(graph_node *node, char **outbuf_ptr){
    if(node->child_num == 0){
        return;
    }
    memcpy(outbuf_ptr[0], node->name, strlen(node->name) * sizeof(char));
    outbuf_ptr[0] += strlen(node->name);
    outbuf_ptr[0][0] = '(';
    outbuf_ptr[0] ++;
    outbuf_ptr[0][0] = ')';
    outbuf_ptr[0] ++;
    outbuf_ptr[0][0] = '{';
    outbuf_ptr[0] += 1;
    outbuf_ptr[0][0] = '\n';
    outbuf_ptr[0] += 1;
    for (int i = 0; i < node->child_num; i ++){
        //Number of function the loop contain 
        int count = 0;
        if(node->loop_nums[i] > 1){
            int j = i;
            while (j < node->child_num){
                if(node->loop_nums[j] > 1){
                    count ++;
                }
                j ++;
            }
        }
        if(count > 0){
            //add loop to outbuf
            char str[50] = {};
            sprintf(str,"    loop(%d){\n",node->loop_nums[i]);
            int len = strlen(str);
            memcpy(outbuf_ptr[0], str, len);
            outbuf_ptr[0] += len;
            for (int j = 0; j < count; j++){
                //add \t\t
                for (int k = 0; k < 8; k ++){
                    outbuf_ptr[0][0] = ' ';
                    outbuf_ptr[0] ++;
                }
                len = strlen(node->childs[i + j]->name);
                memcpy(outbuf_ptr[0], node->childs[i + j]->name, len);
                outbuf_ptr[0] += len;
                outbuf_ptr[0][0] = ';';
                outbuf_ptr[0] += 1;
                outbuf_ptr[0][0] = '\n';
                outbuf_ptr[0] += 1;
            }
            for (int k = 0; k < 4; k ++){
                outbuf_ptr[0][0] = ' ';
                outbuf_ptr[0] ++;
            }
            outbuf_ptr[0][0] = '}';
            outbuf_ptr[0] ++;
            outbuf_ptr[0][0] = '\n';
            outbuf_ptr[0] ++;
            i += count;
        }else{
            for (int k = 0; k < 4; k ++){
                outbuf_ptr[0][0] = ' ';
                outbuf_ptr[0] ++;
            }
            int len = strlen(node->childs[i]->name);
            memcpy(outbuf_ptr[0], node->childs[i]->name, len);
            outbuf_ptr[0] += len;
            outbuf_ptr[0][0] = ';';
            outbuf_ptr[0] += 1;
            outbuf_ptr[0][0] = '\n';
            outbuf_ptr[0] += 1; 
        }

    }
    outbuf_ptr[0][0] = '}';
    outbuf_ptr[0] += 1;
    outbuf_ptr[0][0] = '\n';
    outbuf_ptr[0] += 1;
    outbuf_ptr[0][0] = '\n';
    outbuf_ptr[0] += 1;

    for (int i = 0; i < node->child_num; i ++){
        __parse_call_structure(node->childs[i], outbuf_ptr);
    }
}

/*
* Parse function call structure.
* keep result in outbuf.
*/
void
parse_call_structure(graph *structure_graph, char *outbuf_ptr){
    //Number of function nesting
    __parse_call_structure(structure_graph->root_node, &outbuf_ptr);
}

/*
* Only process 0 executes this function.
* Parse config file and output to file.
*/
void
print_structure(char *config_buf){
    cJSON *config_json = cJSON_Parse(config_buf);
    graph *structure_graph = (graph*) malloc(sizeof(graph));
    char *outbuf = (char*) malloc(sizeof(char) * MAX_FILE_SIZE);
    memset(outbuf, 0, sizeof(char) * MAX_FILE_SIZE);
    generate_call_structure(structure_graph, config_json); 
    parse_call_structure(structure_graph, outbuf);
    
    FILE *file;
    char filename[MAX_FILE_NAME] = "structure.log";
    file = fopen(filename,  "w");
    fprintf(file,"%s",outbuf);

    //clear
    fclose(file);
    free(outbuf);
    clear_graph(structure_graph);
}