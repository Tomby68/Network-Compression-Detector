#include "standalone.h"

// Bring in args from main so it is not redeclared in standalone.h
extern struct arg_struct args[512];

/* Send the UDP packet train to the destination specified in the config struct
 * Will either send a low entropy packet train if high_ent_flag == 0, or a high entropy train if high_ent_flag == 1
 * args:
 * struct config_details config: The config struct containing details about the destination
 * int high_ent_flag: Flag to decide whether to send low or high entropy data
 */
void udp_send(struct config_details config, int high_ent_flag) {
	// Initialize structs for getaddrinfo
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

	// Open socket
	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		printf("Failed to create UDP socket\n");
		error(fd);
	}

	// Bind socket
	err_check = bind(fd, client->ai_addr, client->ai_addrlen);
	if (err_check == -1) {
		printf("Failed to bind socket\n");
		error(errno);
	}
	freeaddrinfo(client);

	/* For zsh/mac systems, use this call */
	int val = 1;
	err_check = setsockopt(fd, IPPROTO_IP, IP_DONTFRAG, &val, sizeof(val));
	/* For linux/ubuntu systems, use this call */
	//int val = IP_PMTUDISC_DO;
	//err_check = setsockopt(fd, IPPROTO_IP, IP_MTU_DISCOVER, &val, sizeof(val));
	if (err_check == -1) {
		error(errno);
	}

	// Initialize UDP packet data buffer
	char buf[1000];
	memset(&buf, 0, sizeof(buf));
	// Check the high_ent_flag: If set, fill buf with high_entropy data. Otherwise, leave it as all 0s
	if (high_ent_flag) {
		// Read high_entropy data from random.txt
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
		printf("about to send high entropy UDP train...\n");
	} else {
		printf("About to send low entropy UDP train...\n");
	}
	// Send config.udp_num UDP packets
	for (int16_t i = 0; i < atoi(config.udp_num); i++) {
		// Add i as packet id
		buf[0] = i >> 8;
		buf[1] = i & 0xFF;
		sendto(fd, buf, atoi(config.udp_payload), 0, res->ai_addr, res->ai_addrlen);
	}
	printf("Successfully sent %s UDP packets\n", config.udp_num);
	freeaddrinfo(res);
	close(fd);
}
