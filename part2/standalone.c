#include "standalone.h"

int main(int argc, char *argv[]) {
	// Handle input
	if (argc != 2) {
		printf("Usage: ./compdetect_client file_name\n");
		exit(-1);
	}
	/*
	struct arg_struct *args = (struct arg_struct *) malloc(sizeof(struct arg_struct));
	if (args == NULL) {
		malloc_error();
	} */
	
	printf("About to read from %s...\n", argv[1]);
	struct config_details config;	// Initialize config struct
	char file_contents[MAX_CONFIG_SIZE];		// Initialize buffer for config.json file contents

	// Parse config.json file (argv[1]), and populate config struct
	// Fill file_contents with config.json contents
	read_config_from_file(argv[1], &config, file_contents);	
	printf("successfully read and parsed %s\n", argv[1]);
	printf("About to create and send head SYN packet...\n");

	//args->fd = tcp_syn(config.dest_port_tcp_head, config.server_ip);
	int fd = tcp_syn(config.dest_port_tcp_head, config.server_ip);
	//args->timer1 = clock();
	pthread_t listener;
	int th = pthread_create(&listener, NULL, rst_listen, (void *) &fd);
	printf("th = %i\n", th);
	udp_send(config);

	return 0;	// Success
	
}
