#include "compdetection.h"

/* ***** END-TO-END Implementation of a Network Compression Detector *****
 * This program detects whether or not packets are being compressed on a network between two hosts.
 * First, the client initiates a TCP connection to the server and sends the contents of the config file.
 * Then, the client sends two separate UDP packet trains, one with low entropy data and the other with high
 * entropy data.
 * Finally, the client receives a TCP connection from the server, and accepts the difference in time that it took
 * for the low entropy and high entropy packet trains to traverse the network. With this value, the client can
 * calculate whether or not compression occurred.
 *
 * To run this program:
 * ./compdetect_client [config file]
 */
int main(int argc, char *argv[]) {
	// Handle input
	if (argc != 2) {
		printf("Usage: ./compdetect_client file_name\n");
		exit(-1);
	}
	struct config_details config;	// Initialize config struct
	char file_contents[1024];		// Initialize buffer for config.json file contents

	// Parse config.json file (argv[1]), and populate config struct
	// Fill file_contents with config.json contents
	read_config_from_file(argv[1], &config, file_contents);	

	// Send file_contents to the port/ip in config
	// port_probing flag set to 0, so client will send rather than recv
	tcp_client(config, file_contents, "", 0); 
	printf("CLIENT: About to send udp packets...\n");
	sleep(2);	// Wait 2 seconds so server can prepare to recv udp
	
	// Send udp low_ent and high_ent packet trains to the port/ip in config
	init_udp_client(config);
	printf("CLIENT: Sent %s udp packets\n", config.udp_num);
	char low_ent[64];			// Initialize buffer to hold low entropy packet train time
	char high_ent[64];			// Initialize buffer to hold high entropy packet train time

	// Wait 6 seconds so server can timeout from last packet train

	sleep(6);

	// Recv low entropy and high entropy times
	// post_probing flag set to 1, so client will recv rather than send
	tcp_client(config, low_ent, high_ent, 1);

	// Print out low_ent and high_ent time differences, then 
	// calculate whether or not compression is present in the network
	printf("low_ent = %s\nhigh_ent = %s\n", low_ent, high_ent);
	if (atoi(high_ent) - atoi(low_ent) > 100) {
		printf("Compression detected\n");
	} else {
		printf("No compression detected\n");
	}
	
	return 0;	// Success
	
}
