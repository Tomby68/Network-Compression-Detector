#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct config_details {
	char server_ip[32];
	int source_port_udp;
	int dest_port_udp;
	int dest_port_tcp_head;
	int dest_port_tcp_tail;
	char port_tcp_pre[32];
	int port_tcp_post;
	int udp_payload;
	int inter_measurement_time;
	int udp_num;
	int udp_ttl;
};

void error_gai(int error);
void error(int err_num);
