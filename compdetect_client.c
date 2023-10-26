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

	init_udp_client(config);
	
	return 0;	// Success
	
}
