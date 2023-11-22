#include <sys/socket.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "cJSON.h"

#define MAX_CONFIG_SIZE 1024
#define ENT_MSG_SIZE 64
#define NLOG 5
#define HIGH_ENT "random.txt"

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

struct ipheader {
	unsigned char iph_v:4, iph_l:4;
	unsigned char iph_tos;
	unsigned short int iph_tl, iph_id, iph_offset;
	unsigned char iph_ttl, iph_proto;
	unsigned short int iph_sum;
	unsigned int iph_src, iph_dst;
};


struct tcpheader {
	unsigned short int th_sport;
	unsigned short int th_dport;
	unsigned int th_seq;
	unsigned int th_ack;
	unsigned char th_x2:4, th_off:4;
	unsigned char th_flags;
	unsigned short int th_win;
	unsigned short int th_sum;
	unsigned short int th_urp;
};

void malloc_error();
void error_gai(int error);
void error(int err_num);
void error_sock();
void error_detail(char *msg);
void jsonError(const char *error, cJSON *json);

void tcp_syn(char *port_num, char *server_ip);
unsigned short checksum(unsigned short *buf, int nwords);

char *read_json_key(cJSON *json, char *name);
void read_config_from_file(char *fileName, struct config_details *config, char *buf);
void read_config(struct config_details *config, char *buf);
