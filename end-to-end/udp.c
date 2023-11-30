#include "compdetection.h"


/* Send two UDP packet trains. The first one consists of UDP packets with low entropy data,
 * and the second train consists of UDP packets with high entropy data.
 * 
 * args:
 * struct config_details config: A struct containing information about the server
 */
void init_udp_client(struct config_details config) {
	// Initialize structs for getaddrinfo()
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *client;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	// getaddrinfo() to get server information
	int err_check = getaddrinfo(config.server_ip, config.dest_port_udp, &hints, &res);
	if (err_check) {
		error_gai(err_check);
	}

	// Another getaddrinfo(), this time to get client information
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	err_check = getaddrinfo(NULL, config.source_port_udp, &hints, &client);
	if (err_check) {
		error_gai(err_check);
	}

	// Open and bind the socket
	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		error(fd);
	}
	err_check = bind(fd, client->ai_addr, client->ai_addrlen);
	if (err_check == -1) {
		error(errno);
	}
	freeaddrinfo(client);
	
	int val = 1;
	err_check = setsockopt(fd, IPPROTO_IP, IP_DONTFRAG, &val, sizeof(val));
	//int val = IP_PMTUDISC_DO;
	//err_check = setsockopt(fd, IPPROTO_IP, IP_MTU_DISCOVER, &val, sizeof(val));
	if (err_check == -1) {
		error(errno);
	}

	// send config.udp_num low entropy packets
	char buf[1000];
	memset(&buf, 0, sizeof(buf));
	printf("About to send low entropy UDP train...\n");
	for (int16_t i = 0; i < atoi(config.udp_num); i++) {
		// Add i as packet id
		buf[0] = i >> 8;
		buf[1] = i & 0xFF;
		sendto(fd, buf, atoi(config.udp_payload), 0, res->ai_addr, res->ai_addrlen);
	}
	printf("Successfully sent %s UDP packets\n", config.udp_num);

	// Wait before sending the high entropy packet train to ensure all packets propagate through the network
	sleep(atoi(config.inter_measurement_time));

	// Get high entropy data from random.txt
	FILE *rand = fopen(HIGH_ENT, "r");
	if (rand == NULL) {
		printf("failed to open %s\n", HIGH_ENT);
		exit(-1);
	}
	int len = fread(buf, 1, atoi(config.udp_payload), rand);
	if (len == 0) {
		if (!feof(rand)) {
			error(ferror(rand));
		}
	}
	fclose(rand);

	// send config.udp_num high entropy packets
	printf("About to send high entropy UDP train...\n");
	for (uint16_t i = 0; i < atoi(config.udp_num); i++) {
		buf[0] = i >> 8;
		buf[1] = i & 0xFF;
		sendto(fd, buf, atoi(config.udp_payload), 0, res->ai_addr, res->ai_addrlen);
	}
	printf("Successfully sent %s UDP packets\n", config.udp_num);
	freeaddrinfo(res);
	close(fd);
}	


/* Accept the two UDP packet trains
 * 
 * args:
 * struct config_details config: The struct containing information on ports to listen to
 */
long init_udp_server(struct config_details config) {
	// Initialize structs for getaddrinfo()
	struct addrinfo hints;
	struct addrinfo *res;
	struct sockaddr_storage client;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	// getaddrinfo() to get server information
	int err_check = getaddrinfo(NULL, config.dest_port_udp, &hints, &res);
	if (err_check) {
		error_gai(err_check);
	}

	// Open and bind the socket
	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		error(fd);
	}
	err_check = bind(fd, res->ai_addr, res->ai_addrlen);
	if (err_check == -1) {
		error(errno);
	}
	freeaddrinfo(res);

	// Set a timeout option of 5 seconds on the socket
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	err_check = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if (err_check == -1) {
		error(errno);
	}

	// Initialize a buffer to hold packet data
	char buf[1024];
	socklen_t addr_len = sizeof(client);
	int total_packets = 0;

	// Initialize the timestamp variables
	long first = 0;
	long last = 0;
	int bytes_recved = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client, &addr_len);
	first = clock();
	total_packets++;

	// Keep accepting UDP packets until the timeout is reached
	while (1) {
		bytes_recved = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client, &addr_len);
		if (bytes_recved == -1) {
			break;
		}
		last = clock();
		total_packets++;
	}
	printf("Packets received: %i\n", total_packets);
	long diff = (last - first) / 1000;
	printf("Time elapsed: %lu\n", diff);
	freeaddrinfo((struct addrinfo *) &client);
	freeaddrinfo(&hints);
	close(fd);
	return diff;
}
