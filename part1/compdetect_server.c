#include "compdetection.h"


int main(int argc, char *argv[]) {
	// Handle input
	if (argc != 2) {
		printf("Usage: ./compdetect_server port_num\n");
		exit(-1);
	}

	char buf[MAX_CONFIG_SIZE];			// Initialize buffer to hold contents of config.json file sent from client

	// Open, listen, and accept on socket from port argv[1]
	// buf will hold what is received, and post_probing flag is set to 0 so server knows to recv rather than send
	tcp_server(argv[1], buf, "", 0);
	
	struct config_details config;		// Initialize config struct to hold contents of the config.json file

	// Parse the file contents stored in buf, and populate the config struct
	read_config(&config, buf);
	
	printf("SERVER: Successfully recved and parsed config file, preparing to recv udp packets...\n");
	// Recv low entropy udp packet train, and store the time elapsed from first to last recved packet in low_diff
	long low_diff = init_udp_server(config);		
	printf("SERVER: Successfully recved low entropy udp packets\n");
	
	// Wait 3 seconds to wait out the rest of the client's inter-measurement time
	sleep(3);
	
	// Recv high entropy udp packet train, and store the time elapsed between first to last recved packet in high_diff
	long high_diff = init_udp_server(config);
	printf("SERVER: Successfully recved high entropy udp packets\n");

	printf("Low entropy time: %lu millisec\nHigh entropy time: %lu millisec\n", low_diff, high_diff);
	// Calculate whether or not compression is present on the network
	if (high_diff - low_diff > 100) {
		printf("Compression detected\n");
	} else {
		printf("No compression detected\n");
	}

	// Convert low_diff and high_diff from unsigned longs to char * so they can be sent to the client
	int low_length = snprintf(NULL, 0, "%lu", low_diff);
	int high_length = snprintf(NULL, 0, "%lu", high_diff);
	char *low_ent = malloc(low_length + 1);
	char *high_ent = malloc(high_length + 1);
	snprintf(low_ent, low_length + 1, "%lu", low_diff);
	snprintf(high_ent, high_length + 1, "%lu", high_diff);

	// Send low_ent and high_ent to the socket accepted on config.port_tcp_post
	// post_probing flag set to 1 so server knows to send time elapsed values rather than recv
	tcp_server(config.port_tcp_post, low_ent, high_ent, 1);
	free(low_ent);
	free(high_ent);
	return 0; // Success
}
