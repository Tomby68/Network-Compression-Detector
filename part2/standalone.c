#include "standalone.h"

/* 
***** What I have working *****
 * Parse config file correctly
 * Construct and send tcp syn head packet 
 * Create a separate thread to listen for RST packets
 * Send the first(?) udp packet train
 * Construct and send tcp tail head packet (CHECK ON VMS)

***** What's left to do *****
 * Pass the head and tail config ports to the listening thread so it knows if an rst packet it
 * receives is from the head or tail syn packet
 * Return and save the difference in time b/w head/tail rst packets in the main thread
 * Send a second packet train: Head syn, high entropy udp, tail syn
 * Calculate compression!

 */

struct arg_struct args[512];

int main(int argc, char *argv[]) {
	// Handle input
	if (argc != 2) {
		printf("Usage: ./compdetect_client file_name\n");
		exit(-1);
	}
	
	printf("About to read from %s...\n", argv[1]);
	struct config_details config;	// Initialize config struct
	char file_contents[MAX_CONFIG_SIZE];		// Initialize buffer for config.json file contents

	// Parse config.json file (argv[1]), and populate config struct
	// Fill file_contents with config.json contents
	read_config_from_file(argv[1], &config, file_contents);	
	printf("successfully read and parsed %s\n", argv[1]);
	printf("About to create and send head SYN packet...\n");

	args->fd = tcp_syn(config.dest_port_tcp_head, config.server_ip);
	args->head_port = config.dest_port_tcp_head;
	args->tail_port = config.dest_port_tcp_tail;

	pthread_t listener;
	pthread_create(&listener, NULL, rst_listen, (void *) &args);
	udp_send(config);

	tcp_syn(config.dest_port_tcp_tail, config.server_ip);
	char *buf;

	printf("Waiting for RST packets...\n");
	int listen = pthread_join(listener, (void **) &buf);
	if (listen != 0) {
		printf("Listener thread failed to join with errno %i\n", listen);
	}
 
	return 0;	// Success
	
}
