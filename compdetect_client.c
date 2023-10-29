#include "compdetection.h"

int main(int argc, char *argv[]) {
	// Handle input
	if (argc != 2) {
		printf("Usage: ./compdetect_client file_name\n");
		exit(-1);
	}
	struct config_details config;

	char file_contents[1024];
	
	read_config_from_file(argv[1], &config, file_contents);

	//init_tcp_client(config, file_contents); 
	printf("CLIENT: About to send udp packets...\n");
	sleep(1);
	init_udp_client(config);
	printf("CLIENT: Sent %s udp packets\n", config.udp_num);
	
	return 0;	// Success
	
}
