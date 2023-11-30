#include "standalone.h"

// Bring in args from standalone.c
extern struct arg_struct args[512];

/* Send one packet train: A head TCP SYN packet, followed by a UDP packet train,
 * followed by a tail TCP SYN packet
 * At the same time, create a listener thread to capture RST packets while sending UDP packets
 *
 * args: 
 * struct config_details config: A struct containing the elements from the given config.json file
 * int high_ent_flag: Flag determining whether to send low or high entropy UDP packets
 */
long packet_train(struct config_details config, int high_ent_flag) {
	// Prepare args for the listener thread by filling in the file descriptor, head_port, and tail_port
	// Also, send the head TCP SYN packet
	args->fd = tcp_syn(config.dest_port_tcp_head, config.server_ip);
	args->head_port = config.dest_port_tcp_head;
	args->tail_port = config.dest_port_tcp_tail;
	
	// Initialize and start listener thread
	pthread_t listener;
	pthread_create(&listener, NULL, rst_listen, (void *) &args);

	// Send UDP packet train
	udp_send(config, high_ent_flag);
	// Send the tail TCP SYN packet
	tcp_syn(config.dest_port_tcp_tail, config.server_ip);

	// Wait for any remaining RST packets
	char *buf;
	printf("Waiting for RST packets...\n");
	int listen = pthread_join(listener, (void **) &buf);
	if (listen != 0) {
		printf("Listener thread failed to join with errno %i\n", listen);
	}
	// Return difference between head and tail RST receive times
	return args->difference;
}
