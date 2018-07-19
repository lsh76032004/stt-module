#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>


#include "info.h"
#include "../include/parson.h"

#define PID		"stt"

#define BUFFER_SIZE	1024

static char pid_buffer[BUFFER_SIZE];
static char cmd_buffer[BUFFER_SIZE];

enum CMD {
	HANDSHAKE = 0,
	START_SERVICE,
	STOP_SERVICE,
	GET_STT,
};

pthread_t tid;
bool run;

/*
 *
	input : full packet string(json format)
	sample : {"key1","value1" : "key2","value2"}
*/
int read_json(char* json_packet)
{
	int ret = 0;
	/*Init json object*/
	JSON_Value *rootValue;
	JSON_Object *rootObject;

	rootValue = json_parse_string(json_packet);
	rootObject = json_value_get_object(rootValue);

	strncpy(pid_buffer, json_object_get_string(rootObject, "pid"), BUFFER_SIZE);
	strncpy(cmd_buffer, json_object_get_string(rootObject, "cmd"), BUFFER_SIZE);

	if (strcmp(pid_buffer, PID) != 0 && strcmp(pid_buffer, "global")) {
		ret = -1;
		goto OUT;
	}

	if (strcmp(cmd_buffer, "handshake") == 0) {
		ret = HANDSHAKE;
	}else if (strcmp(cmd_buffer, "start_stt") == 0) {
		ret = START_SERVICE;
	}else if (strcmp(cmd_buffer, "stop_stt") == 0) {
		ret = STOP_SERVICE;
	}else if (strcmp(cmd_buffer, "get_data") == 0) {
		ret = GET_STT;
	}

	/*Get value function*/
	printf("[key : %s] [data : %s]\n", "pid", json_object_get_string(rootObject, "pid"));
	printf("[key : %s] [data : %s]\n", "cmd", json_object_get_string(rootObject, "cmd"));


	/*free memory*/
	json_value_free(rootValue);

	/* APPENDIX
	//Get array value 
	int i;
	JSON_Array *array = json_object_get_array(rootObject, "key#");
	for(i=0; i<json_array_get_count(array); i++)
	{
		printf("%s\n", json_array_get_string(array, i);	
	}
	*/

OUT:
	return ret;
}

/*
	make json packet & print
 */
void response_handshake(char *send_buf)
{
	JSON_Value *rootValue;
	JSON_Object *rootObject;
	char *buf;

	/*init empty json packet*/
	rootValue = json_value_init_object();
	rootObject = json_value_get_object(rootValue);

	/*add key & value*/
	json_object_set_string(rootObject, "pid", pid_buffer);
	json_object_set_string(rootObject, "cmd", cmd_buffer);
	json_object_set_string(rootObject, "module", PID);

	/*get full string of json packet */
	buf =  json_serialize_to_string(rootValue);
	strncpy(send_buf, buf, BUFFER_SIZE);
	printf("result json : %s\n", buf);

	//free memory
	json_free_serialized_string(buf);
	json_value_free(rootValue);
}

/*
	make json packet & print
 */
void response_start(char *send_buf)
{
	JSON_Value *rootValue;
	JSON_Object *rootObject;
	char *buf;
	char stt_buf[BUFFER_SIZE];

	/*init empty json packet*/
	rootValue = json_value_init_object();
	rootObject = json_value_get_object(rootValue);

	/*add key & value*/
	json_object_set_string(rootObject, "pid", pid_buffer);
	json_object_set_string(rootObject, "cmd", cmd_buffer);

	/*get full string of json packet */
	buf =  json_serialize_to_string(rootValue);
	strncpy(send_buf, buf, BUFFER_SIZE);
	printf("result json : %s\n", buf);

	//free memory
	json_free_serialized_string(buf);
	json_value_free(rootValue);
}

/*
	make json packet & print
 */
void response_stop(char *send_buf)
{
	JSON_Value *rootValue;
	JSON_Object *rootObject;
	char *buf;
	char stt_buf[BUFFER_SIZE];

	/*init empty json packet*/
	rootValue = json_value_init_object();
	rootObject = json_value_get_object(rootValue);

	/*add key & value*/
	json_object_set_string(rootObject, "pid", pid_buffer);
	json_object_set_string(rootObject, "cmd", cmd_buffer);

	/*get full string of json packet */
	buf =  json_serialize_to_string(rootValue);
	strncpy(send_buf, buf, BUFFER_SIZE);
	printf("result json : %s\n", buf);

	//free memory
	json_free_serialized_string(buf);
	json_value_free(rootValue);
}

/*
	make json packet & print
 */
void response_msg(char *send_buf)
{
	JSON_Value *rootValue;
	JSON_Object *rootObject;
	char *buf;
	char stt_buf[BUFFER_SIZE];

	printf("response msg\n");
	FILE *fp = fopen("/root/Project/stt.log", "r+");
	if (fp == NULL) {
		stt_buf[0] = '\0';
	}else {
		fgets(stt_buf, BUFFER_SIZE, fp);
		printf(stt_buf);
		fclose(fp);	
	}

	/*init empty json packet*/
	rootValue = json_value_init_object();
	rootObject = json_value_get_object(rootValue);

	/*add key & value*/
	json_object_set_string(rootObject, "pid", pid_buffer);
	json_object_set_string(rootObject, "cmd", cmd_buffer);
	json_object_set_string(rootObject, "data", stt_buf);

	/*get full string of json packet */
	buf =  json_serialize_to_string(rootValue);
	strncpy(send_buf, buf, BUFFER_SIZE);
	printf("result json : %s\n", buf);

	//free memory
	json_free_serialized_string(buf);
	json_value_free(rootValue);
}

void *run_stt(void *data)
{
	printf("start stt google\n");
	system("python3 /root/STT/python-docs-samples/speech/cloud-client/stt.py");
}

void stt_module_init(struct info_t *info)
{
	printf("[%s] stt Module is initialized!!!\n", __func__);
}

void stt_module(struct info_t *info)
{
	char send_buffer[BUFFER_SIZE];

	printf("[%s] RECV: %s!\n", __func__, info->receive_msg);

	int ret = read_json(info->receive_msg);
	if (ret < 0) {
		return;
	}

	switch (ret) {
	case HANDSHAKE:
		printf("handshake\n");
		response_handshake(send_buffer);	

		info->send(info, send_buffer);
		break;
	case START_SERVICE:
		printf("start service\n");
		run = true;
		response_start(send_buffer);
		pthread_create(&tid, NULL, run_stt, NULL);
		break;
	case STOP_SERVICE:
		printf("stop service\n");
		response_stop(send_buffer);
		run = false;
		pthread_cancel(tid);
		break;
	case GET_STT:
		response_msg(send_buffer);
		printf("send: %s\n", send_buffer);
		((struct info_t*)info)->send(info, send_buffer);
	default:
		break;
	}
}
