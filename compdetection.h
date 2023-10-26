#include <sys/socket.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"

#define MAX_CONFIG_SIZE 1024
#define NLOG 5

struct config_details {
	char *server_ip;
	char *source_port_udp;
	char *dest_port_udp;
	char *dest_port_tcp_head;
	char *dest_port_tcp_tail;
	char *port_tcp_pre;
	char *port_tcp_post;
	char *udp_payload;
	char *inter_measurement_time;
	char *udp_num;
	char *udp_ttl;
};

void malloc_error();
void error_gai(int error);
void error(int err_num);
void error_sock();
void error_detail(char *msg);
void jsonError(const char *error, cJSON *json);

void init_tcp_client(struct config_details config, char *file_contents);
void init_tcp_server(char *port_num, char *buf);

void init_udp_client(struct config_details config);
void init_udp_server(struct config_details config);

char *read_json_key(cJSON *json, char *name);
void read_config_from_file(char *fileName, struct config_details *config, char *buf);
void read_config(struct config_details *config, char *buf);