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
	if (err_check == -1) {
		error(errno);
	}

	// set some sock opts

	// send config.udp_num packets
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
	// sleep(config.inter_measurement_time)
	// send config.udp_num packets
	/*int bytes_sent = sendto(fd, config.udp_payload, strlen(config.udp_payload), 0, res->ai_addr, res->ai_addrlen);
	printf("sent: %s\nlength: %lu\n", config.udp_payload, strlen(config.udp_payload));
	if (bytes_sent == -1) {
		error(errno);
	} */
	freeaddrinfo(res);
	close(fd);
}	

void init_udp_server(struct config_details config) {
	struct addrinfo hints;
	struct addrinfo *res;
	struct sockaddr_storage client;
	struct timeval first_time;
	struct timeval last_time;
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

	char buf[1024];
	socklen_t addr_len = sizeof(client);

	int total_packets = 0;
	char *packet_id = "";

	int bytes_recved = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client, &addr_len);
	gettimeofday(&first_time, NULL);
	total_packets++;
	packet_id = strncpy(packet_id, buf, 2);
	while (atoi(packet_id) != atoi(config.udp_num) - 1 && total_packets < 6000) {
		bytes_recved = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client, &addr_len);
		gettimeofday(&last_time, NULL);
		total_packets++;
		packet_id = strncpy(packet_id, buf, 16);
	}
	printf("Packets received: %i\n", total_packets);

	buf[bytes_recved] = '\0';
	printf("last recved over udp: buf = %s\n", buf);
}
