#include "compdetection.h"


int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Usage: ./compdetect_server port_num\n");
		exit(-1);
	}

	char buf[1024];
	init_tcp_server(argv[1], buf);
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

	return 0; // Success
}
