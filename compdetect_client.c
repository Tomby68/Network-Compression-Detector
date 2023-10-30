#include "compdetection.h"

int main(int argc, char *argv[]) {
	// Handle input
	char *buf = "";
	char buf1[64];
	printf("sizeof(buf) = %lu\n", sizeof(buf));
	printf("sizeof(buf1) = %lu\n", sizeof(buf1));
	if (argc != 2) {
		printf("Usage: ./compdetect_client file_name\n");
		exit(-1);
	}
	struct config_details config;

	char file_contents[1024];
	
	read_config_from_file(argv[1], &config, file_contents);

	tcp_send(config, file_contents, "", config.server_ip); 
	printf("CLIENT: About to send udp packets...\n");
	sleep(2);
	init_udp_client(config);
	printf("CLIENT: Sent %s udp packets\n", config.udp_num);
	char low_ent[64];
	char high_ent[64];
	tcp_recv(config.port_tcp_post, low_ent, high_ent);
	printf("low_ent = %s\nhigh_ent = %s\n", low_ent, high_ent);
	if (atoi(high_ent) - atoi(low_ent) > 100) {
		printf("Compression detected\n");
	} else {
		printf("No compression detected\n");
	}
	
	
	return 0;	// Success
	
}
