#include "compdetection.h"


int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Usage: ./compdetect_server port_num\n");
		exit(-1);
	}

	char buf[1024];
	struct sockaddr client_ip = tcp_recv(argv[1], buf, "");
	struct config_details config;
	read_config(&config, buf);
	printf("SERVER: Successfully recved and parsed config file, preparing to recv udp packets...\n");
	long low_diff = init_udp_server(config);
	printf("SERVER: Successfully recved low entropy udp packets\n");
	sleep(3);
	long high_diff = init_udp_server(config);
	printf("SERVER: Successfully recved high entropy udp packets\n");

	printf("Low entropy time: %lu millisec\nHigh entropy time: %lu millisec\n", low_diff, high_diff);
	if (high_diff - low_diff > 100) {
		printf("Compression detected\n");
	} else {
		printf("No compression detected\n");
	}
	int low_length = snprintf(NULL, 0, "%lu", low_diff);
	int high_length = snprintf(NULL, 0, "%lu", high_diff);
	char *low_ent = malloc(low_length + 1);
	char *high_ent = malloc(high_length + 1);
	snprintf(low_ent, low_length + 1, "%lu", low_diff);
	snprintf(high_ent, high_length + 1, "%lu", high_diff);
	tcp_send(config, low_ent, high_ent, client_ip.sa_data);
	free(low_ent);
	free(high_ent);
	return 0; // Success
}
