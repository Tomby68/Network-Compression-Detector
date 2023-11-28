#include "standalone.h"

void udp_send(struct config_details config) {
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *client;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	int err_check = getaddrinfo(config.server_ip, config.dest_port_udp, &hints, &res);
	if (err_check) {
		error_gai(err_check);
	}
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	err_check = getaddrinfo(NULL, config.source_port_udp, &hints, &client);
	if (err_check) {
		error_gai(err_check);
	}

	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		error(fd);
	}

	err_check = bind(fd, client->ai_addr, client->ai_addrlen);
	if (err_check == -1) {
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

	freeaddrinfo(res);
	close(fd);

}
