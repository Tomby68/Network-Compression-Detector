#include "compdetection.h"

void init_udp_client(struct config_details config) {
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

	sleep(atoi(config.inter_measurement_time));
	// send config.udp_num high entropy packets

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

void init_udp_server(struct config_details config) {
	struct addrinfo hints;
	struct addrinfo *res;
	struct sockaddr_storage client;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	int err_check = getaddrinfo(NULL, config.dest_port_udp, &hints, &res);
	if (err_check) {
		error_gai(err_check);
	}

	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		error(fd);
	}

	err_check = bind(fd, res->ai_addr, res->ai_addrlen);
	if (err_check == -1) {
		error(errno);
	}
	freeaddrinfo(res);

	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	err_check = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if (err_check == -1) {
		error(errno);
	}

	char buf[1024];
	socklen_t addr_len = sizeof(client);

	int total_packets = 0;
	long first = 0;
	long last = 0;
	int bytes_recved = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client, &addr_len);
	first = clock();
	total_packets++;
	while (1) {
		bytes_recved = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client, &addr_len);
		if (bytes_recved == -1) {
			break;
		}
		last = clock();
		total_packets++;
	}
	printf("Packets received: %i\n", total_packets);
	long low_diff = (last - first) / 1000;
	printf("Time elapsed: %lu\n", low_diff);

	sleep(3);
	
	printf("Ready to receive next packet train");
	total_packets = 0;
	bytes_recved = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client, &addr_len);
	first = clock();
	total_packets++;
	while(1) {
		bytes_recved = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client, &addr_len);
		if (bytes_recved == 0) {
			break;
		} 
		last = clock();
		total_packets++;
	}
	printf("Packets received: %i\n", total_packets);
	long high_diff = (last - first) / 1000;
	printf("Time elapsed: %lu\n", high_diff);
}
