#include "standalone.h"

long packet_train(struct config_details config, int high_ent_flag) {
	args->fd = tcp_syn(config.dest_port_tcp_head, config.server_ip);
	args->head_port = config.dest_port_tcp_head;
	args->tail_port = config.dest_port_tcp_tail;

	pthread_t listener;
	pthread_create(&listener, NULL, rst_listen, (void *) &args);
	udp_send(config, high_ent_flag);

	tcp_syn(config.dest_port_tcp_tail, config.server_ip);
	
	char *buf;
	printf("Waiting for RST packets...\n");
	int listen = pthread_join(listener, (void **) &buf);
	if (listen != 0) {
		printf("Listener thread failed to join with errno %i\n", listen);
	}
	return args->difference;
}
